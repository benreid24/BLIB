#include <BLIB/Engine/Window.hpp>
#include <BLIB/Media.hpp>
#include <SFML/Graphics.hpp>

int main() {
    bl::engine::Window<sf::RenderWindow> window;
    window.create(
        sf::VideoMode(800, 600, 32), "BLIB Shapes Demo", sf::Style::Close | sf::Style::Titlebar);

    bl::shapes::Triangle triangle({0.f, 5.f}, {-5.f, 0.f}, {5.f, 0.f});
    triangle.setFillColor(sf::Color::Green);
    triangle.setOutlineColor(sf::Color::Red);
    triangle.setOutlineThickness(1.f);
    triangle.setPosition(100.f, 100.f);
    triangle.setScale(10.f, 10.f);

    bl::shapes::Ellipse ellipse(120.f, 60.f);
    ellipse.setPosition(500.f, 300.f);
    ellipse.setFillColor(sf::Color::Yellow);
    ellipse.setOutlineThickness(2.f);
    ellipse.setOutlineColor(sf::Color::Red);

    bl::shapes::GradientCircle circle(50.f);
    circle.setCenterColor(sf::Color::Black);
    circle.setOuterColor(sf::Color::Red);
    circle.setPosition(100.f, 400.f);

    bl::shapes::GradientRectangle rect({200.f, 100.f},
                                       bl::shapes::GradientRectangle::BottomToTop,
                                       sf::Color::Red,
                                       sf::Color(255, 0, 0, 0));
    rect.setPosition(350.f, 400.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            if (event.type == sf::Event::LostFocus) {
                while (window.waitEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        break;
                    }
                    if (event.type == sf::Event::GainedFocus) { break; }
                }
            }
        }

        triangle.rotate(1.f);
        ellipse.rotate(-2.f);

        window.getSfWindow().clear(sf::Color::Cyan);
        window.getSfWindow().draw(triangle);
        window.getSfWindow().draw(ellipse);
        window.getSfWindow().draw(circle);
        window.getSfWindow().draw(rect);
        window.getSfWindow().display();

        sf::sleep(sf::milliseconds(16));
    }

    return 0;
}
