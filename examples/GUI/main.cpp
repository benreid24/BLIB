#include <BLIB/GUI.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Util/EventDispatcher.hpp>

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

void b1click(const bl::gui::Action&, bl::gui::Element*) {
    std::cout << "Button b1 was clicked\n";
}

void b2click(const bl::gui::Action&, bl::gui::Element*) {
    std::cout << "Button b2 was clicked\n";
}

void b3click(const bl::gui::Action&, bl::gui::Element*) {
    std::cout << "Button b3 was clicked\n";
}

void renderStuff(sf::RenderTexture& target) {
    sf::RectangleShape rect(static_cast<sf::Vector2f>(target.getSize()));
    rect.setFillColor(sf::Color::Blue);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(-3);
    target.draw(rect);

    const sf::Vector2f center(static_cast<sf::Vector2f>(target.getSize()) * 0.5f);
    const float r = static_cast<float>(std::min(target.getSize().x, target.getSize().y)) * 0.4;
    sf::CircleShape circle(r);
    circle.setPosition(center);
    circle.setOrigin(r, r);
    circle.setFillColor(sf::Color(170, 60, 60));
    circle.setOutlineColor(sf::Color::Black);
    circle.setOutlineThickness(1);
    target.draw(circle);

    const float r2 = r * 0.8;
    circle.setOutlineThickness(0);
    circle.setRadius(3);
    circle.setOrigin(3, 3);
    for (int i = 0; i < 360; ++i) {
        const float a = i;
        const int c   = a / 360.f * 255.f;
        circle.setFillColor(sf::Color(c, c, c));
        circle.setPosition(center + sf::Vector2f(r2 * std::cos(a / 180 * 3.1415),
                                                 r2 * std::sin(a / 180 * 3.1415)));
        target.draw(circle);
    }
    target.display();
}

int main() {
    using namespace bl;

    sf::RenderWindow window(
        sf::VideoMode(800, 600, 32), "BLIB GUI Demo", sf::Style::Close | sf::Style::Titlebar);

    TextureResourceManager textureManager;

    WindowEventDispatcher dispatcher;
    GUI::Ptr gui = GUI::create(
        gui::LinePacker::create(gui::LinePacker::Vertical, 4, gui::LinePacker::Compact),
        {200, 100, 400, 400},
        "",
        "gui");
    gui::DebugRenderer::Ptr renderer = gui::DebugRenderer::create();
    dispatcher.subscribe(gui.get());
    gui->setRenderer(renderer);

    gui::Image::Ptr image = gui::Image::create(textureManager.load("image.png"));
    image->setFillAcquisition(true, true);
    gui->pack(image, true, true);

    gui->pack(gui::Separator::create(gui::Separator::Horizontal));

    gui::Label::Ptr label = gui::Label::create("This a label", "labels", "l1");
    label->setColor(sf::Color::Red, sf::Color::Transparent);
    gui->pack(label, true, true);

    gui->pack(gui::Separator::create(gui::Separator::Horizontal));

    label = gui::Label::create("This another label", "labels", "l2");
    gui->pack(label);

    gui::Button::Ptr button = gui::Button::create("Press Me", "buttons", "b1");
    button->getSignal(gui::Action::LeftClicked).willCall(b1click);
    gui->pack(button, true, true);

    gui::Window::Ptr testWindow =
        gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                            "Test Window",
                            gui::Window::Default,
                            {30, 30},
                            "",
                            "window1");
    label = gui::Label::create("This is a window");
    testWindow->pack(label);
    button = gui::Button::create("Click me");
    button->getSignal(gui::Action::LeftClicked).willCall(b2click);
    testWindow->pack(button);
    testWindow->getSignal(gui::Action::Closed)
        .willCall(std::bind(&gui::Element::remove, testWindow.get()));
    gui->pack(testWindow);

    gui::Slider::Ptr slider = gui::Slider::create(gui::Slider::Horizontal);
    slider->setRequisition({120, 35});
    testWindow->pack(slider);

    testWindow = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                     "Test Window",
                                     gui::Window::Default,
                                     {200, 30},
                                     "",
                                     "window2");
    label      = gui::Label::create("This is also a window");
    testWindow->pack(label);
    button = gui::Button::create("Click me too");
    button->getSignal(gui::Action::LeftClicked).willCall(b3click);
    testWindow->pack(button);
    testWindow->getSignal(gui::Action::Closed)
        .willCall(std::bind(&gui::Element::remove, testWindow.get()));
    gui->pack(testWindow);

    gui::Canvas::Ptr canvas = gui::Canvas::create(100, 75);
    renderStuff(canvas->getTexture());
    testWindow->pack(canvas);

    testWindow = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                     "Scroll Window",
                                     gui::Window::Default,
                                     {200, 300},
                                     "",
                                     "window3");
    gui::ScrollArea::Ptr scroll =
        gui::ScrollArea::create(gui::LinePacker::create(gui::LinePacker::Vertical, 5));
    scroll->setMaxSize({150, 100});
    scroll->pack(gui::Label::create("This can be scrolled"));
    scroll->pack(gui::Label::create("Try scrolling me around"));
    scroll->pack(gui::Button::create("No Action"));
    testWindow->pack(scroll, true, true);
    gui->pack(testWindow);

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