#include <BLIB/GUI.hpp>
#include <BLIB/Util/EventDispatcher.hpp>

#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(
        sf::VideoMode(800, 600, 32), "BLIB GUI Demo", sf::Style::Close | sf::Style::Titlebar);

    bl::WindowEventDispatcher dispatcher;
    bl::GUI::Ptr gui = bl::GUI::create(bl::gui::LinePacker::create(), window, "gui");
    dispatcher.subscribe(gui.get());

    bl::gui::Label::Ptr label = bl::gui::Label::create("This a label", "label");
    gui->add(label);

    sf::Clock timer;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            dispatcher.dispatch(event);
        }

        gui->update(timer.restart().asSeconds());

        window.clear(sf::Color(90, 90, 90));
        window.draw(*gui);
        window.display();

        sf::sleep(sf::milliseconds(30));
    }

    return 0;
}