#include <BLIB/Shapes.hpp>
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600, 32),
                            "BLIB Shapes Demo",
                            sf::Style::Close | sf::Style::Titlebar);

    bl::Triangle triangle({0, 5}, {-5, 0}, {5, 0});
    triangle.setFillColor(sf::Color::Green);
    triangle.setOutlineColor(sf::Color::Red);
    triangle.setOutlineThickness(1);
    triangle.setPosition(100, 100);
    triangle.setScale(10, 10);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        triangle.rotate(1);

        window.clear(sf::Color::Cyan);
        window.draw(triangle);
        window.display();

        sf::sleep(sf::milliseconds(16));
    }

    return 0;
}