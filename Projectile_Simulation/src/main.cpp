#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <optional>
#include <sstream>
#include <string>

#include "ProjectileSim.h"

namespace {
constexpr float kPanelWidth = 260.f;
constexpr float kPanelPadding = 20.f;
constexpr float kSliderWidth = 200.f;
constexpr float kSliderHeight = 6.f;
constexpr float kKnobRadius = 9.f;
constexpr float kButtonWidth = 96.f;
constexpr float kButtonHeight = 34.f;

float clampf(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

bool containsPoint(const sf::FloatRect& rect, sf::Vector2f point) {
    return rect.contains(point);
}

bool tryLoadFont(sf::Font& font) {
    const std::array<std::string, 4> candidates = {
        "assets/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };

    for (const auto& path : candidates) {
        if (font.openFromFile(path)) {
            return true;
        }
    }
    return false;
}
}

struct UiButton {
    sf::RectangleShape rect;
    std::optional<sf::Text> label;
    bool hovered = false;
};

struct UiSlider {
    sf::RectangleShape track;
    sf::CircleShape knob;
    std::optional<sf::Text> label;
    std::optional<sf::Text> valueText;
    float minValue = 0.f;
    float maxValue = 1.f;
    float value = 0.f;
    bool dragging = false;
};

int main() {
    const unsigned int width = 1000;
    const unsigned int height = 700;

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(width, height)), "Projectile Simulator");
    window.setFramerateLimit(120);

    const float radius = 12.f;
    const sf::Vector2f origin(100.f, 600.f);

    ProjectileSim sim(origin, origin.y, radius);
    sim.setGravity(500.f);

    float speed = 350.f;
    float angleDeg = 55.f;

    sf::CircleShape ball(radius);
    ball.setFillColor(sf::Color::Green);
    ball.setOrigin(sf::Vector2f(radius, radius));

    sf::RectangleShape cannon(sf::Vector2f(80.f, 10.f));
    cannon.setFillColor(sf::Color::White);
    cannon.setOrigin(sf::Vector2f(0.f, 5.f));
    cannon.setPosition(origin);

    sf::RectangleShape ground(sf::Vector2f(static_cast<float>(width), 4.f));
    ground.setFillColor(sf::Color(150, 150, 150));
    ground.setPosition(sf::Vector2f(0.f, origin.y + radius));

    sf::Font font;
    const bool hasFont = tryLoadFont(font);

    const float panelX = static_cast<float>(width) - kPanelWidth;
    sf::RectangleShape panel(sf::Vector2f(kPanelWidth, static_cast<float>(height)));
    panel.setPosition(sf::Vector2f(panelX, 0.f));
    panel.setFillColor(sf::Color(20, 20, 20));
    panel.setOutlineThickness(1.f);
    panel.setOutlineColor(sf::Color(60, 60, 60));

    UiSlider velocitySlider;
    velocitySlider.minValue = 50.f;
    velocitySlider.maxValue = 800.f;
    velocitySlider.value = speed;
    velocitySlider.track.setSize(sf::Vector2f(kSliderWidth, kSliderHeight));
    velocitySlider.track.setFillColor(sf::Color(110, 110, 110));
    velocitySlider.track.setPosition(sf::Vector2f(panelX + kPanelPadding, 90.f));
    velocitySlider.knob.setRadius(kKnobRadius);
    velocitySlider.knob.setOrigin(sf::Vector2f(kKnobRadius, kKnobRadius));
    velocitySlider.knob.setFillColor(sf::Color(230, 230, 230));

    UiSlider angleSlider;
    angleSlider.minValue = 5.f;
    angleSlider.maxValue = 85.f;
    angleSlider.value = angleDeg;
    angleSlider.track.setSize(sf::Vector2f(kSliderWidth, kSliderHeight));
    angleSlider.track.setFillColor(sf::Color(110, 110, 110));
    angleSlider.track.setPosition(sf::Vector2f(panelX + kPanelPadding, 190.f));
    angleSlider.knob.setRadius(kKnobRadius);
    angleSlider.knob.setOrigin(sf::Vector2f(kKnobRadius, kKnobRadius));
    angleSlider.knob.setFillColor(sf::Color(230, 230, 230));

    UiButton launchButton;
    launchButton.rect.setSize(sf::Vector2f(kButtonWidth, kButtonHeight));
    launchButton.rect.setPosition(sf::Vector2f(panelX + kPanelPadding, 260.f));
    launchButton.rect.setFillColor(sf::Color(70, 130, 70));

    UiButton resetButton;
    resetButton.rect.setSize(sf::Vector2f(kButtonWidth, kButtonHeight));
    resetButton.rect.setPosition(sf::Vector2f(panelX + kPanelPadding + kButtonWidth + 16.f, 260.f));
    resetButton.rect.setFillColor(sf::Color(130, 70, 70));

    if (hasFont) {
        velocitySlider.label.emplace(font, "Velocity", 16);
        velocitySlider.label->setFillColor(sf::Color::White);
        velocitySlider.label->setPosition(sf::Vector2f(panelX + kPanelPadding, 60.f));

        velocitySlider.valueText.emplace(font, "", 14);
        velocitySlider.valueText->setFillColor(sf::Color(200, 200, 200));

        angleSlider.label.emplace(font, "Angle (deg)", 16);
        angleSlider.label->setFillColor(sf::Color::White);
        angleSlider.label->setPosition(sf::Vector2f(panelX + kPanelPadding, 160.f));

        angleSlider.valueText.emplace(font, "", 14);
        angleSlider.valueText->setFillColor(sf::Color(200, 200, 200));

        launchButton.label.emplace(font, "Launch", 16);
        launchButton.label->setFillColor(sf::Color::White);

        resetButton.label.emplace(font, "Reset", 16);
        resetButton.label->setFillColor(sf::Color::White);
    }

    auto updateSliderKnob = [](UiSlider& slider) {
        const sf::Vector2f pos = slider.track.getPosition();
        const float width = slider.track.getSize().x;
        const float t = (slider.value - slider.minValue) / (slider.maxValue - slider.minValue);
        slider.knob.setPosition(sf::Vector2f(pos.x + t * width, pos.y + slider.track.getSize().y * 0.5f));
    };

    auto updateSliderValueFromMouse = [](UiSlider& slider, float mouseX) {
        const float left = slider.track.getPosition().x;
        const float width = slider.track.getSize().x;
        const float t = clampf((mouseX - left) / width, 0.f, 1.f);
        slider.value = slider.minValue + t * (slider.maxValue - slider.minValue);
    };

    updateSliderKnob(velocitySlider);
    updateSliderKnob(angleSlider);

    sf::Clock deltaClock;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* data = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (data->button == sf::Mouse::Button::Left) {
                    const sf::Vector2f mousePos = window.mapPixelToCoords(data->position);

                    if (containsPoint(launchButton.rect.getGlobalBounds(), mousePos)) {
                        sim.reset(velocitySlider.value, angleSlider.value);
                    }

                    if (containsPoint(resetButton.rect.getGlobalBounds(), mousePos)) {
                        sim.stop();
                    }

                    if (containsPoint(velocitySlider.track.getGlobalBounds(), mousePos) ||
                        containsPoint(velocitySlider.knob.getGlobalBounds(), mousePos)) {
                        velocitySlider.dragging = true;
                        updateSliderValueFromMouse(velocitySlider, mousePos.x);
                        updateSliderKnob(velocitySlider);
                    }

                    if (containsPoint(angleSlider.track.getGlobalBounds(), mousePos) ||
                        containsPoint(angleSlider.knob.getGlobalBounds(), mousePos)) {
                        angleSlider.dragging = true;
                        updateSliderValueFromMouse(angleSlider, mousePos.x);
                        updateSliderKnob(angleSlider);
                    }
                }
            }

            if (const auto* data = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (data->button == sf::Mouse::Button::Left) {
                    velocitySlider.dragging = false;
                    angleSlider.dragging = false;
                }
            }

            if (const auto* data = event->getIf<sf::Event::MouseMoved>()) {
                const sf::Vector2f mousePos = window.mapPixelToCoords(data->position);

                if (velocitySlider.dragging) {
                    updateSliderValueFromMouse(velocitySlider, mousePos.x);
                    updateSliderKnob(velocitySlider);
                }
                if (angleSlider.dragging) {
                    updateSliderValueFromMouse(angleSlider, mousePos.x);
                    updateSliderKnob(angleSlider);
                }
            }
        }

        float dt = deltaClock.restart().asSeconds();
        dt = std::min(dt, 0.033f);

        speed = velocitySlider.value;
        angleDeg = angleSlider.value;

        const sf::Vector2i mouse = sf::Mouse::getPosition(window);
        const sf::Vector2f mousePos = window.mapPixelToCoords(mouse);
        launchButton.hovered = containsPoint(launchButton.rect.getGlobalBounds(), mousePos);
        resetButton.hovered = containsPoint(resetButton.rect.getGlobalBounds(), mousePos);

        launchButton.rect.setFillColor(launchButton.hovered ? sf::Color(90, 160, 90) : sf::Color(70, 130, 70));
        resetButton.rect.setFillColor(resetButton.hovered ? sf::Color(160, 90, 90) : sf::Color(130, 70, 70));

        if (hasFont && velocitySlider.valueText && angleSlider.valueText) {
            std::ostringstream velocityStream;
            velocityStream << static_cast<int>(speed) << " px/s";
            velocitySlider.valueText->setString(velocityStream.str());
            velocitySlider.valueText->setPosition(sf::Vector2f(panelX + kPanelPadding, 110.f));

            std::ostringstream angleStream;
            angleStream << static_cast<int>(angleDeg) << " deg";
            angleSlider.valueText->setString(angleStream.str());
            angleSlider.valueText->setPosition(sf::Vector2f(panelX + kPanelPadding, 210.f));

            if (launchButton.label) {
                const sf::FloatRect launchBounds = launchButton.label->getLocalBounds();
                launchButton.label->setPosition(
                    sf::Vector2f(
                        launchButton.rect.getPosition().x + (kButtonWidth - launchBounds.size.x) * 0.5f - launchBounds.position.x,
                        launchButton.rect.getPosition().y + (kButtonHeight - launchBounds.size.y) * 0.5f - launchBounds.position.y
                    )
                );
            }

            if (resetButton.label) {
                const sf::FloatRect resetBounds = resetButton.label->getLocalBounds();
                resetButton.label->setPosition(
                    sf::Vector2f(
                        resetButton.rect.getPosition().x + (kButtonWidth - resetBounds.size.x) * 0.5f - resetBounds.position.x,
                        resetButton.rect.getPosition().y + (kButtonHeight - resetBounds.size.y) * 0.5f - resetBounds.position.y
                    )
                );
            }
        }

        sim.update(dt);

        ball.setPosition(sim.position());
        cannon.setRotation(sf::degrees(-angleDeg));

        window.clear(sf::Color::Black);
        window.draw(ground);
        window.draw(cannon);
        window.draw(ball);
        window.draw(panel);

        if (hasFont) {
            if (velocitySlider.label) {
                window.draw(*velocitySlider.label);
            }
            if (velocitySlider.valueText) {
                window.draw(*velocitySlider.valueText);
            }
            if (angleSlider.label) {
                window.draw(*angleSlider.label);
            }
            if (angleSlider.valueText) {
                window.draw(*angleSlider.valueText);
            }
        }

        window.draw(velocitySlider.track);
        window.draw(velocitySlider.knob);
        window.draw(angleSlider.track);
        window.draw(angleSlider.knob);
        window.draw(launchButton.rect);
        window.draw(resetButton.rect);

        if (hasFont) {
            if (launchButton.label) {
                window.draw(*launchButton.label);
            }
            if (resetButton.label) {
                window.draw(*resetButton.label);
            }
        }

        window.display();
    }
    return 0;
}
