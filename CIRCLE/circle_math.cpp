#include <SFML/Graphics.hpp>
#include <vector>



int main(){
    const int width = 800;
    const int height = 600;

    sf::RenderWindow window(sf::VideoMode({width, height}), "Manual Circle");


    const int cx = 400;   // center x
    const int cy = 300;   // center y
    const int r = 100;    // radius

    sf::Image image;
    image.resize({width, height}, sf::Color::Black);


    // Then we render the points 
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dx = x - cx;
            int dy = y - cy;
            // well this is much simpler then using directly the sqrt
            if (dx * dx + dy * dy <= r * r) {
                image.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, sf::Color::Green);
            }
        }
    }
    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite(texture);

    // then the untold close loop
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }

    return 0; // return the goddamn 0 babyyy 
}