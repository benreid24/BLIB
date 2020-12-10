#include <BLIB/Menu.hpp>
#include <BLIB/Util/EventDispatcher.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::Font font;
    font.loadFromFile("font.ttf");

    sf::Texture texture;
    texture.loadFromFile("title.png");

    bl::menu::BasicRenderer renderer;
    bl::menu::ArrowSelector::Ptr selector = bl::menu::ArrowSelector::create(12);

    bl::menu::Item::Ptr title =
        bl::menu::Item::create(bl::menu::SpriteRenderItem::create(sf::Sprite(texture)));

    bl::menu::Item::Ptr newGame =
        bl::menu::Item::create(bl::menu::TextRenderItem::create(sf::Text("New Game", font)));
    newGame->getSignal(bl::menu::Item::Activated).willCall([]() {
        std::cout << "New Game\n";
    });

    bl::menu::Item::Ptr loadGame =
        bl::menu::Item::create(bl::menu::TextRenderItem::create(sf::Text("Load Game", font)));
    loadGame->getSignal(bl::menu::Item::Activated).willCall([]() {
        std::cout << "Load Game\n";
    });

    bl::menu::Item::Ptr quit =
        bl::menu::Item::create(bl::menu::TextRenderItem::create(sf::Text("Quit", font)));

    title->attach(newGame, bl::menu::Item::Bottom);
    title->setSelectable(false);
    newGame->attach(loadGame, bl::menu::Item::Bottom);
    loadGame->attach(quit, bl::menu::Item::Bottom);

    bl::menu::Menu menu(newGame, selector);
    bl::menu::KeyboardEventGenerator keyboardEventGenerator(menu);
    bl::menu::MouseEventGenerator mouseEventGenerator(menu);

    bl::WindowEventDispatcher eventDispatcher;
    eventDispatcher.subscribe(&keyboardEventGenerator);
    eventDispatcher.subscribe(&mouseEventGenerator);

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
            eventDispatcher.dispatch(event);
        }

        window.clear();
        menu.render(renderer, window, {350, 150});
        window.display();

        sf::sleep(sf::milliseconds(20));
    }

    return 0;
}
