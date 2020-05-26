#include <BLIB/GUI.hpp>
#include <BLIB/Util/EventDispatcher.hpp>

#include <SFML/Graphics.hpp>
#include <iostream>

void b1click(const bl::gui::Action&, bl::gui::Element*) {
    std::cout << "Button b1 was clicked\n";
}

int main() {
    using namespace bl;

    sf::RenderWindow window(
        sf::VideoMode(800, 600, 32), "BLIB GUI Demo", sf::Style::Close | sf::Style::Titlebar);

    WindowEventDispatcher dispatcher;
    GUI::Ptr gui = GUI::create(
        gui::LinePacker::create(gui::LinePacker::Vertical, 4, gui::LinePacker::Uniform),
        {200, 100, 400, 400},
        "",
        "gui");
    gui::DebugRenderer::Ptr renderer = gui::DebugRenderer::create();
    dispatcher.subscribe(gui.get());
    gui->setRenderer(renderer);

    gui::Label::Ptr label = gui::Label::create("This a label", "labels", "l1");
    label->setColor(sf::Color::Red, sf::Color::Transparent);
    gui->add(label, true, true);

    label = gui::Label::create("This another label", "labels", "l2");
    gui->add(label);

    gui::Button::Ptr button = gui::Button::create("Press Me", "buttons", "b1");
    button->getSignal(gui::Action::LeftClicked).willCall(b1click);
    gui->add(button, true, true);

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