#include <BLIB/Events/Dispatcher.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <BLIB/Resources.hpp>

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

struct TextureLoader : public bl::resource::Loader<sf::Texture> {
    virtual ~TextureLoader() = default;

    virtual bl::resource::Resource<sf::Texture>::Ref load(const std::string& uri) override {
        bl::resource::Resource<sf::Texture>::Ref ref(new sf::Texture());
        if (!ref->loadFromFile(uri)) {
            BL_LOG_ERROR << "Failed to load texture from file: " << uri;
        }
        return ref;
    }
} textureLoader;

void b1click(const bl::gui::Event&, bl::gui::Element*) { std::cout << "Button b1 was clicked\n"; }

void b2click(const bl::gui::Event&, bl::gui::Element*) { std::cout << "Button b2 was clicked\n"; }

void b3click(const bl::gui::Event&, bl::gui::Element*) { std::cout << "Button b3 was clicked\n"; }

void notebookCb() { std::cout << "Button inside of notebook was clicked\n"; }

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

    resource::Manager<sf::Texture> textureManager(textureLoader);

    event::Dispatcher dispatcher;
    gui::GUI::Ptr gui = gui::GUI::create(
        gui::LinePacker::create(gui::LinePacker::Vertical, 4, gui::LinePacker::Compact),
        {200, 100, 400, 400});
    gui::DebugRenderer::Ptr renderer = gui::DebugRenderer::create();
    gui->subscribe(dispatcher);
    gui->setRenderer(renderer);

    gui->setOutlineThickness(1);
    gui->setColor(sf::Color::Transparent, sf::Color::Red);

    gui::Image::Ptr image = gui::Image::create(textureManager.load("image.png").data);
    image->setFillAcquisition(true, true);
    gui->pack(image, true, true);

    gui->pack(gui::Separator::create(gui::Separator::Horizontal));

    gui::Label::Ptr label = gui::Label::create("This a label");
    label->setColor(sf::Color::Red, sf::Color::Transparent);
    gui->pack(label, true, true);

    gui->pack(gui::Separator::create(gui::Separator::Horizontal));

    gui::ComboBox::Ptr combo = gui::ComboBox::create();
    combo->addOption("Option 1");
    combo->addOption("Option 2");
    combo->addOption("Option 3");
    combo->addOption("Option 4");
    combo->addOption("Option 5");
    combo->setMaxHeight(50);
    gui->pack(combo);

    const float ProgressPerSecond     = 0.1;
    gui::ProgressBar::Ptr progressBar = gui::ProgressBar::create();
    progressBar->setRequisition({10, 20});
    gui->pack(progressBar, true, false);

    label = gui::Label::create("This another label");
    gui->pack(label);

    gui::Button::Ptr button = gui::Button::create("Press Me");
    button->getSignal(gui::Event::LeftClicked).willCall(b1click);
    gui->pack(button, true, true);

    gui::Window::Ptr testWindow =
        gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                            "Test Window",
                            gui::Window::Default,
                            {30, 30});
    label = gui::Label::create("This is a window");
    testWindow->pack(label);
    button = gui::Button::create("Click me");
    button->getSignal(gui::Event::LeftClicked).willCall(b2click);
    testWindow->pack(button);
    testWindow->getSignal(gui::Event::Closed)
        .willCall(std::bind(&gui::Element::remove, testWindow.get()));
    // gui->pack(testWindow);

    gui::Slider::Ptr slider = gui::Slider::create(gui::Slider::Horizontal);
    slider->setRequisition({120, 35});
    testWindow->pack(slider);

    testWindow = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                     "Test Window",
                                     gui::Window::Default,
                                     {200, 30});
    label      = gui::Label::create("This is also a window");
    testWindow->pack(label);
    button = gui::Button::create("Click me too");
    button->getSignal(gui::Event::LeftClicked).willCall(b3click);
    testWindow->pack(button);
    testWindow->getSignal(gui::Event::Closed)
        .willCall(std::bind(&gui::Element::remove, testWindow.get()));
    // gui->pack(testWindow);

    gui::Canvas::Ptr canvas = gui::Canvas::create(100, 75);
    renderStuff(canvas->getTexture());
    testWindow->pack(canvas);

    testWindow = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                     "Scroll Window",
                                     gui::Window::Default,
                                     {200, 300});
    gui::ScrollArea::Ptr scroll =
        gui::ScrollArea::create(gui::LinePacker::create(gui::LinePacker::Vertical, 5));
    scroll->setMaxSize({150, 100});
    scroll->pack(gui::Label::create("This can be scrolled"));
    scroll->pack(gui::Label::create("Try scrolling me around"));
    scroll->pack(gui::Button::create("No Event"));
    slider = gui::Slider::create(gui::Slider::Horizontal);
    slider->setRequisition({120, 35});
    scroll->pack(slider);
    testWindow->pack(scroll, true, true);
    // gui->pack(testWindow);

    testWindow = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                     "Text Window",
                                     gui::Window::Default,
                                     {150, 200});
    gui::TextEntry::Ptr entry = gui::TextEntry::create(4);
    entry->setRequisition({100, 20});
    testWindow->pack(entry, true, true);
    // gui->pack(testWindow);

    testWindow            = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                     "Notebook Window",
                                     gui::Window::Default,
                                     {10, 200});
    gui::Notebook::Ptr nb = gui::Notebook::create();
    nb->addPage("page1", "Page 1", gui::Label::create("Content goes here"));
    gui::Box::Ptr box = gui::Box::create(gui::LinePacker::create(gui::LinePacker::Vertical));
    button            = gui::Button::create("Content");
    button->getSignal(gui::Event::LeftClicked).willCall(std::bind(&notebookCb));
    box->pack(button);
    box->pack(gui::Label::create("This is a notebook"));
    nb->addPage("page2", "More Stuff", box);
    box = gui::Box::create(gui::LinePacker::create(gui::LinePacker::Vertical));
    box->pack(gui::CheckButton::create("Check me"));
    box->pack(gui::CheckButton::create("Or me"));
    box->pack(gui::CheckButton::create("Maybe me"));
    nb->addPage("page3", "Checkboxes", box);
    box = gui::Box::create(gui::LinePacker::create(gui::LinePacker::Vertical));
    gui::RadioButton::Ptr radio = gui::RadioButton::create("Check me");
    box->pack(radio);
    box->pack(gui::RadioButton::create("Or me", radio->getRadioGroup()));
    box->pack(gui::RadioButton::create("Could be me", radio->getRadioGroup()));
    nb->addPage("page4", "Radio buttons", box);
    testWindow->pack(nb, true, true);
    // gui->pack(testWindow);

    bool showBoxes = false;
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
            }
            dispatcher.dispatch(event);
        }

        progressBar->setProgress(progressBar->getProgress() +
                                 timer.getElapsedTime().asSeconds() * ProgressPerSecond);
        gui->update(timer.restart().asSeconds());

        window.clear(sf::Color(90, 90, 90));
        window.draw(*gui);
        window.display();

        sf::sleep(sf::milliseconds(30));
    }

    return 0;
}