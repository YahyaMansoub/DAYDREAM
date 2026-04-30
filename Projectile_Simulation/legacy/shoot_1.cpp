#include <SFML/Graphics.hpp>
#include <cmath>
#include <optional>

int main() {
    const unsigned int width = 1000;
    const unsigned int height = 700;

    sf::RenderWindow window(sf::VideoMode({width, height}), "Cannon Shot - Loop");
 
    const float radius = 12.f;

    const float x0 = 100.f;
    const float y0 = 600.f;

    const float g = 500.f;
    const float speed = 350.f;
    const float angleDeg = 55.f;
    const float angleRad = angleDeg * 3.14159265f / 180.f;

    const float v0x = speed * std::cos(angleRad);
    const float v0y = -speed * std::sin(angleRad);
    // THE BALL BABY
    sf::CircleShape ball(radius);
    ball.setFillColor(sf::Color::Green);
    ball.setOrigin({radius, radius});

    sf::RectangleShape cannon(sf::Vector2f(80.f, 10.f));
    cannon.setFillColor(sf::Color::White);
    cannon.setOrigin({0.f, 5.f});
    cannon.setPosition({x0, y0});
    cannon.setRotation(sf::degrees(-angleDeg));

    sf::RectangleShape ground(sf::Vector2f(static_cast<float>(width), 4.f));
    ground.setFillColor(sf::Color(150, 150, 150));
    ground.setPosition({0.f, y0 + radius});

    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        float t = clock.getElapsedTime().asSeconds();
        // The update is not what is usually used but i wanted to try it cuz like it's what i've seen in physics courses hhh
        float x = x0 + v0x * t;
        float y = y0 + v0y * t + 0.5f * g * t * t;

        if (y >= y0) {
            clock.restart();
            x = x0;
            y = y0;
        }

        ball.setPosition({x, y});

        window.clear(sf::Color::Black);
        window.draw(ground);
        window.draw(cannon);
        window.draw(ball);
        window.display();
    }

    return 0;
}
