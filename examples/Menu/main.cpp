#include <BENG/Menu.hpp>
#include <BENG/Util/EventDispatcher.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::Font font;
    font.loadFromFile("font.ttf");

    sf::Texture texture;
    texture.loadFromFile("title.png");

    bg::menu::BasicRenderer renderer;
    bg::menu::ArrowSelector::Ptr selector = bg::menu::ArrowSelector::create(12);

    bg::menu::Item::Ptr title =
        bg::menu::Item::create(bg::menu::SpriteRenderItem::create(sf::Sprite(texture)));

    bg::menu::Item::Ptr newGame =
        bg::menu::Item::create(bg::menu::TextRenderItem::create(sf::Text("New Game", font)));
    newGame->getSignal(bg::menu::Item::Activated).willCall([]() {
        std::cout << "New Game\n";
    });

    bg::menu::Item::Ptr loadGame =
        bg::menu::Item::create(bg::menu::TextRenderItem::create(sf::Text("Load Game", font)));
    loadGame->getSignal(bg::menu::Item::Activated).willCall([]() {
        std::cout << "Load Game\n";
    });

    bg::menu::Item::Ptr quit =
        bg::menu::Item::create(bg::menu::TextRenderItem::create(sf::Text("Quit", font)));

    title->attach(newGame, bg::menu::Item::Bottom);
    title->setSelectable(false);
    newGame->attach(loadGame, bg::menu::Item::Bottom);
    loadGame->attach(quit, bg::menu::Item::Bottom);

    bg::menu::Menu menu(newGame, selector);
    bg::menu::KeyboardEventGenerator keyboardEventGenerator(menu);
    bg::menu::MouseEventGenerator mouseEventGenerator(menu);

    bg::WindowEventDispatcher eventDispatcher;
    eventDispatcher.subscribe(&keyboardEventGenerator);
    eventDispatcher.subscribe(&mouseEventGenerator);

    sf::RenderWindow window(
        sf::VideoMode(800, 600, 32), "Menu Demo", sf::Style::Close | sf::Style::Titlebar);

    quit->getSignal(bg::menu::Item::Activated).willCall([&window]() { window.close(); });

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
