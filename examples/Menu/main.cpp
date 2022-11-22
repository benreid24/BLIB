#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace bl::menu;

int main() {
    sf::Font font;
    font.loadFromFile("font.ttf");

    bl::resource::Resource<sf::Texture>::Ref texture =
        bl::engine::Resources::textures().load("title.png").data;

    ArrowSelector::Ptr selector = ArrowSelector::create(12);
    Menu menu(selector);

    Item::Ptr title = ImageItem::create(texture);
    title->setSelectable(false);
    title->setAllowSelectionCrossing(true);

    Item::Ptr newGame = TextItem::create("New Game", font, sf::Color::White);
    newGame->getSignal(Item::Activated).willCall([]() { std::cout << "New Game\n"; });

    Item::Ptr loadGame = TextItem::create("Load Game", font, sf::Color::White);
    loadGame->getSignal(Item::Activated).willCall([]() { std::cout << "Load Game\n"; });

    SubmenuItem::Ptr submenu = SubmenuItem::create(
        menu, TextItem::create("Open me", font, sf::Color::White), Item::Right, Item::Bottom);
    ToggleTextItem::Ptr toggleItem = ToggleTextItem::create("Check me", font, sf::Color::White);
    toggleItem->setBoxProperties(sf::Color(50, 50, 50), sf::Color::White, 20.f, 2.f, 12.f, false);
    submenu->addOption(TextItem::create("Submenu option 1", font, sf::Color::White));
    submenu->addOption(TextItem::create("Submenu option 2", font, sf::Color::White));
    submenu->addOption(toggleItem);
    submenu->addOption(TextItem::create("Back", font, sf::Color::White), true);

    Item::Ptr quit     = TextItem::create("Quit", font, sf::Color::White);
    Item::Ptr skipDemo = TextItem::create("Skip to me", font, sf::Color::White);
    Item::Ptr upHere   = TextItem::create("Up here", font, sf::Color::White);

    menu.setRootItem(title);
    menu.addItem(newGame, title.get(), Item::Bottom);
    menu.addItem(loadGame, newGame.get(), Item::Bottom);
    menu.addItem(submenu, loadGame.get(), Item::Bottom);
    menu.addItem(quit, submenu.get(), Item::Bottom);
    menu.addItem(skipDemo, title.get(), Item::AttachPoint::Top);
    menu.addItem(upHere, skipDemo.get(), Item::AttachPoint::Top);
    menu.setSelectedItem(newGame.get());
    menu.setPosition({320.f, 100.f});
    menu.setPadding({30.f, 8.f});

    KeyboardDriver keyboardEventGenerator(menu);
    MouseDriver mouseEventGenerator(menu);

    sf::RenderWindow window(
        sf::VideoMode(800, 600, 32), "Menu Demo", sf::Style::Close | sf::Style::Titlebar);

    quit->getSignal(Item::Activated).willCall([&window]() { window.close(); });

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
