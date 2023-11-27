#include <BLIB/Cameras.hpp>
#include <BLIB/Components.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <BLIB/Render.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

using namespace bl;

constexpr float ProgressPerSecond = 0.1f;

void b2click(const gui::Event&, gui::Element*) { std::cout << "Button b2 was clicked\n"; }

void b3click(const gui::Event&, gui::Element*) { std::cout << "Button b3 was clicked\n"; }

void notebookCb() { std::cout << "Button inside of notebook was clicked\n"; }

class DemoState : public engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All)
    , filePicker("Resources", {"png"},
                 std::bind(&DemoState::onFilePick, this, std::placeholders::_1),
                 [this]() { filePicker.close(); }) {}

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(engine::Engine& engine) override {
        engine.renderer().getObserver().pushScene<rc::Overlay>();
        engine.renderer().getObserver().setClearColor(bl::sfcol({90, 90, 90}));

        gui = gui::GUI::create(
            engine,
            engine.renderer().getObserver(),
            gui::LinePacker::create(gui::LinePacker::Vertical, 4, gui::LinePacker::Compact),
            {200, 100, 400, 400});

        gui->setOutlineThickness(1);
        gui->setColor(sf::Color::Transparent, sf::Color::Red);

        gui::Image::Ptr image =
            gui::Image::create(resource::ResourceManager<sf::Image>::load("Resources/image.png"));
        image->setFillAcquisition(true, true);
        gui->pack(image, true, true);

        gui->pack(gui::Separator::create(gui::Separator::Horizontal));

        gui::Label::Ptr label = gui::Label::create("This a label");
        label->setColor(sf::Color::Red, sf::Color::Transparent);
        gui->pack(label, true, true);

        gui->pack(gui::Separator::create(gui::Separator::Horizontal));

        gui::Box::Ptr row = gui::Box::create(gui::LinePacker::create(gui::LinePacker::Horizontal));
        row->pack(gui::Label::create("Combobox:"));
        gui::ComboBox::Ptr combo = gui::ComboBox::create();
        combo->setTooltip("ComboBoxes allow users to select one of several options");
        combo->addOption("Option 1");
        combo->addOption("Option 2");
        combo->addOption("Option 3");
        combo->addOption("Option 4");
        combo->addOption("Option 5");
        combo->setMaxHeight(70);
        row->pack(combo);
        gui->pack(row);

        progressBar = gui::ProgressBar::create();
        progressBar->setRequisition({10, 20});
        gui->pack(progressBar, true, false);

        label = gui::Label::create("This another label");
        gui->pack(label);

        gui::Button::Ptr button = gui::Button::create("Press Me");
        button->getSignal(gui::Event::LeftClicked)
            .willCall(std::bind(&DemoState::openFilePicker, this));
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
        gui->pack(testWindow);

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
        gui->pack(testWindow);

        gui::Canvas::Ptr canvas = gui::Canvas::create(100, 75);
        rc::SceneRef scene      = engine.renderer().scenePool().allocateScene<rc::scene::Scene2D>();
        canvas->setScene(scene);
        canvas->setClearColor(sf::Color(20, 150, 230));
        canvas->setTooltip("Entire scenes can be rendered to GUI elements!");
        testWindow->pack(canvas);
        spinRect.create(engine, {60.f, 25.f});
        spinRect.getTransform().setOrigin(spinRect.getSize() * 0.5f);
        spinRect.getTransform().setPosition({50.f, 37.5f});
        canvas->setCamera<cam::Camera2D>(glm::vec2{50.f, 37.5f}, glm::vec2{100.f, 75.f});
        engine.ecs().emplaceComponent<com::Velocity2D>(
            spinRect.entity(), glm::vec2{0.f, 0.f}, 180.f);
        spinRect.setHorizontalColorGradient(bl::sfcol(sf::Color::Red), bl::sfcol(sf::Color::Green));
        spinRect.setOutlineColor(bl::sfcol(sf::Color::Black));
        spinRect.addToScene(scene.get(), bl::rc::UpdateSpeed::Dynamic);

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
        gui->pack(testWindow);

        testWindow = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                         "Text Window",
                                         gui::Window::Default,
                                         {150, 200});
        gui::CheckButton::Ptr check = gui::CheckButton::create("Keep focused");
        gui::Window* tmp            = testWindow.get();
        check->getSignal(gui::Event::ValueChanged)
            .willAlwaysCall(
                [tmp](const gui::Event& e, gui::Element*) { tmp->setForceFocus(e.toggleValue()); });
        testWindow->pack(check);
        gui::TextEntry::Ptr entry = gui::TextEntry::create(4);
        entry->setRequisition({100, 20});
        testWindow->pack(entry, true, true);
        gui->pack(testWindow);

        testWindow = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                         "SelectBox Window",
                                         gui::Window::Default,
                                         {450, 200});
        gui::SelectBox::Ptr selectBox = gui::SelectBox::create();
        selectBox->addOption("Select me");
        selectBox->addOption("Or me");
        selectBox->addOption("Could even be me");
        testWindow->pack(selectBox);
        gui->pack(testWindow);

        testWindow = gui::Window::create(gui::LinePacker::create(gui::LinePacker::Vertical),
                                         "Notebook Window",
                                         gui::Window::Default,
                                         {10, 200});
        gui::Notebook::Ptr nb = gui::Notebook::create();
        nb->setTooltip("Notebooks allow re-using the same space for lots of content");
        nb->setMaxTabWidth(150.f);
        nb->addPage("page1", "Page 1", gui::Label::create("Content goes here"));
        gui::Box::Ptr box = gui::Box::create(gui::LinePacker::create(gui::LinePacker::Vertical));
        button            = gui::Button::create("Content");
        button->getSignal(gui::Event::LeftClicked).willCall(std::bind(&notebookCb));
        box->pack(button);
        box->pack(gui::Label::create("This is a notebook"));
        nb->addPage("page2.0",
                    "Animation",
                    gui::Animation::create(resource::ResourceManager<gfx::a2d::AnimationData>::load(
                        "Resources/water.anim")));
        nb->addPage("page2", "More Stuff", box);
        box = gui::Box::create(gui::LinePacker::create(gui::LinePacker::Vertical));
        box->pack(gui::CheckButton::create("Check me"));
        box->pack(gui::CheckButton::create("Or me"));
        box->pack(gui::CheckButton::create("Maybe me"));
        nb->addPage("page3", "Checkboxes", box);
        box = gui::Box::create(gui::LinePacker::create(gui::LinePacker::Vertical));
        gui::RadioButton::Ptr radio = gui::RadioButton::create("Check me", "ex1");
        box->pack(radio);
        box->pack(gui::RadioButton::create("Or me", "ex2", radio->getRadioGroup()));
        box->pack(gui::RadioButton::create("Could be me", "ex3", radio->getRadioGroup()));
        nb->addPage("page4", "Radio buttons", box);
        testWindow->pack(nb, true, true);
        gui->pack(testWindow);

        gui->addToOverlay();
    }

    virtual void deactivate(engine::Engine& engine) override {
        engine.renderer().getObserver().popScene();
    }

    virtual void update(engine::Engine&, float dt, float) override {
        progressBar->setProgress(progressBar->getProgress() + dt * ProgressPerSecond);
        gui->update(dt);
    }

private:
    gui::GUI::Ptr gui;
    gui::ProgressBar::Ptr progressBar;
    gfx::Rectangle spinRect;
    gui::FilePicker filePicker;

    void openFilePicker() {
        filePicker.open(gui::FilePicker::PickExisting, "Select an image", gui);
    }

    void onFilePick(const std::string& file) {
        std::cout << "Selected file '" << file << "'\n";
        filePicker.close();
    }
};

int main() {
    bl::cam::OverlayCamera::setOverlayCoordinateSpace(800.f, 600.f);

    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(800, 600, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar)
            .withTitle("GUI Demo")
            .withLetterBoxOnResize(true));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
