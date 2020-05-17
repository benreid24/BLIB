#include <BLIB/GUI.hpp>
#include <BLIB/Util/EventDispatcher.hpp>

#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(
        sf::VideoMode(800, 600, 32), "BLIB GUI Demo", sf::Style::Close | sf::Style::Titlebar);

    bl::WindowEventDispatcher dispatcher;
    bl::GUI::Ptr gui = bl::GUI::create(
        bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical), window, "", "gui");
    bl::gui::DebugRenderer::Ptr renderer = bl::gui::DebugRenderer::create();
    dispatcher.subscribe(gui.get());
    gui->setRenderer(renderer);

    bl::gui::Label::Ptr label = bl::gui::Label::create("This a label", "labels", "l1");
    gui->add(label);

    label = bl::gui::Label::create("This another label", "labels", "l2");
    gui->add(label);

    bool showBoxes  = false;
    bool showGroups = false;
    bool showIds    = false;

    sf::Clock timer;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            else if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Z) {
                    showBoxes = !showBoxes;
                    renderer->showAcquisitions(showBoxes);
                }
                else if (event.key.code == sf::Keyboard::X) {
                    showGroups = !showGroups;
                    renderer->showGroups(showGroups);
                }
                else if (event.key.code == sf::Keyboard::C) {
                    showIds = !showIds;
                    renderer->showIds(showIds);
                }
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