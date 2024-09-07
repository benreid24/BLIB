#include <BLIB/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Render.hpp>
#include <BLIB/Resources.hpp>
#include <iostream>

using namespace bl::menu;

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All)
    , keyboardEventGenerator(menu)
    , mouseEventGenerator(menu) {}

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        engine.getPlayer().enterWorld<bl::engine::BasicWorld<bl::rc::Overlay>>();

        font.loadFromFile("font.ttf");
        auto texture = engine.renderer().texturePool().getOrLoadTexture("title.png");

        ArrowSelector::Ptr selector = ArrowSelector::create(12, sf::Color::White);
        menu.create(engine, engine.renderer().getObserver(), selector);

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
        toggleItem->setBoxProperties(
            sf::Color(50, 50, 50), sf::Color::White, 20.f, 2.f, 12.f, false);
        submenu->addOption(TextItem::create("Submenu option 1", font, sf::Color::White));
        submenu->addOption(TextItem::create("Submenu option 2", font, sf::Color::White));
        submenu->addOption(toggleItem);
        submenu->addOption(TextItem::create("Back", font, sf::Color::White), true);

        Item::Ptr quit     = TextItem::create("Quit", font, sf::Color::White);
        Item::Ptr skipDemo = TextItem::create("Skip to me", font, sf::Color::White);
        Item::Ptr upHere   = TextItem::create("Up here", font, sf::Color::White);

        quit->getSignal(Item::Activated).willCall([&engine]() { engine.popState(); });

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

        menu.configureBackground(sf::Color::Black, sf::Color::White, 3.f, {15.f, 15.f, 15.f, 15.f});

        menu.addToOverlay();
        bl::event::Dispatcher::subscribe(&keyboardEventGenerator);
        bl::event::Dispatcher::subscribe(&mouseEventGenerator);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        bl::event::Dispatcher::unsubscribe(&keyboardEventGenerator);
        bl::event::Dispatcher::unsubscribe(&mouseEventGenerator);
        engine.getPlayer().leaveWorld();
    }

    virtual void update(bl::engine::Engine&, float, float) override {
        // nothing
    }

private:
    sf::VulkanFont font;
    Menu menu;
    KeyboardDriver keyboardEventGenerator;
    MouseDriver mouseEventGenerator;
};

int main() {
    bl::cam::OverlayCamera::setOverlayCoordinateSpace(800.f, 600.f);

    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(800, 600, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("Menu Demo")
            .withLetterBoxOnResize(true));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
