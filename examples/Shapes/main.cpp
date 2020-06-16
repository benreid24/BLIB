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

    bl::Ellipse ellipse(120, 60);
    ellipse.setPosition(500, 300);
    ellipse.setFillColor(sf::Color::Yellow);
    ellipse.setOutlineThickness(2);
    ellipse.setOutlineColor(sf::Color::Red);

    bl::GradientCircle circle(50);
    circle.setCenterColor(sf::Color::Black);
    circle.setOuterColor(sf::Color::Red);
    circle.setPosition(100, 400);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
        }

        triangle.rotate(1);
        ellipse.rotate(-2);

        window.clear(sf::Color::Cyan);
        window.draw(triangle);
        window.draw(ellipse);
        window.draw(circle);
        window.display();

        sf::sleep(sf::milliseconds(16));
    }

    return 0;
}