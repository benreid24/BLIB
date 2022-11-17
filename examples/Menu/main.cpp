#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::Font font;
    font.loadFromFile("font.ttf");

    bl::resource::Resource<sf::Texture>::Ref texture =
        bl::engine::Resources::textures().load("title.png").data;

    bl::menu::ArrowSelector::Ptr selector = bl::menu::ArrowSelector::create(12);

    bl::menu::Item::Ptr title = bl::menu::ImageItem::create(texture);
    title->setSelectable(false);
    title->setAllowSelectionCrossing(true);

    bl::menu::Item::Ptr newGame = bl::menu::TextItem::create("New Game", font, sf::Color::White);
    newGame->getSignal(bl::menu::Item::Activated).willCall([]() { std::cout << "New Game\n"; });

    bl::menu::Item::Ptr loadGame = bl::menu::TextItem::create("Load Game", font, sf::Color::White);
    loadGame->getSignal(bl::menu::Item::Activated).willCall([]() { std::cout << "Load Game\n"; });

    bl::menu::Item::Ptr quit     = bl::menu::TextItem::create("Quit", font, sf::Color::White);
    bl::menu::Item::Ptr skipDemo = bl::menu::TextItem::create("Skip to me", font, sf::Color::White);
    bl::menu::Item::Ptr upHere   = bl::menu::TextItem::create("Up here", font, sf::Color::White);

    bl::menu::Menu menu(selector);
    menu.setRootItem(title);
    menu.addItem(newGame, title.get(), bl::menu::Item::Bottom);
    menu.addItem(loadGame, newGame.get(), bl::menu::Item::Bottom);
    menu.addItem(quit, loadGame.get(), bl::menu::Item::Bottom);
    menu.addItem(skipDemo, title.get(), bl::menu::Item::AttachPoint::Top);
    menu.addItem(upHere, skipDemo.get(), bl::menu::Item::AttachPoint::Top);
    menu.setSelectedItem(newGame.get());
    menu.setPosition({350.f, 150.f});

    bl::menu::KeyboardDriver keyboardEventGenerator(menu);
    bl::menu::MouseDriver mouseEventGenerator(menu);

    sf::RenderWindow window(
        sf::VideoMode(800, 600, 32), "Menu Demo", sf::Style::Close | sf::Style::Titlebar);

    quit->getSignal(bl::menu::Item::Activated).willCall([&window]() { window.close(); });

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            keyboardEventGenerator.observe(event);
            mouseEventGenerator.observe(event);
        }

        window.clear();
        menu.render(window);
        window.display();

        sf::sleep(sf::milliseconds(20));
    }

    return 0;
}
