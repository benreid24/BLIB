#include <BLIB/Interfaces/GUI/Elements/Slider.hpp>

#include <BLIB/Interfaces/GUI/Elements/Icon.hpp>
#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
namespace
{
constexpr float ArrowWidth  = 32.f;
constexpr float ArrowHeight = 32.f;
} // namespace

Slider::Ptr Slider::create(Direction dir) { return Ptr(new Slider(dir)); }

Slider::Slider(Direction d)
: CompositeElement<3>()
, dir(d)
, sliderSizeRatio(0.2f)
, value(0.f)
, increment(0.1f)
, slider(Button::create("")) {
    Icon::Ptr incIcon = Icon::create(Icon::Type::Arrow, {ArrowWidth, ArrowHeight});
    incIcon->setRotation(d == Direction::Horizontal ? 90.f : 180.f);
    incIcon->setFillAcquisition(true, false);
    increaseBut = Button::create(incIcon);

    Icon::Ptr decIcon = Icon::create(Icon::Type::Arrow, {ArrowWidth, ArrowHeight});
    decIcon->setRotation(d == Direction::Horizontal ? 270.f : 0.f);
    decIcon->setFillAcquisition(true, false);
    decreaseBut = Button::create(decIcon);

    increaseBut->setExpandsHeight(true);
    increaseBut->setExpandsWidth(true);
    decreaseBut->setExpandsHeight(true);
    decreaseBut->setExpandsWidth(true);
    slider->setExpandsHeight(true);
    slider->setExpandsWidth(true);

    using namespace std::placeholders;
    getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Slider::clicked, this, _1));
    slider->getSignal(Event::Dragged).willAlwaysCall(std::bind(&Slider::sliderMoved, this, _1));
    increaseBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&Slider::incrementValue, this, 1.f));
    decreaseBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&Slider::incrementValue, this, -1.f));

    Element* childs[3] = {increaseBut.get(), decreaseBut.get(), slider.get()};
    registerChildren(childs);
}

float Slider::getValue() const { return value; }

void Slider::setValue(float v, bool fire) {
    value = v;
    constrainValue();
    updateSliderPos();
    if (fire) { fireChanged(); }
}

void Slider::setSliderSize(float s) {
    sliderSizeRatio = s;
    if (sliderSizeRatio <= 0.f)
        sliderSizeRatio = 0.01f;
    else if (sliderSizeRatio > 1.f)
        sliderSizeRatio = 1.f;
    onAcquisition();
}

void Slider::setSliderIncrement(float i) { increment = i; }

Button::Ptr Slider::getSlider() { return slider; }

Button::Ptr Slider::getIncreaseButton() { return increaseBut; }

Button::Ptr Slider::getDecreaseButton() { return decreaseBut; }

sf::Vector2f Slider::minimumRequisition() const {
    const sf::Vector2f max(
        std::max(std::max(slider->getAcquisition().width, increaseBut->getAcquisition().width),
                 decreaseBut->getAcquisition().width),
        std::max(std::max(slider->getAcquisition().height, increaseBut->getAcquisition().height),
                 decreaseBut->getAcquisition().height));
    const sf::Vector2f sum(slider->getAcquisition().width + increaseBut->getAcquisition().width +
                               decreaseBut->getAcquisition().width,
                           slider->getAcquisition().height + increaseBut->getAcquisition().height +
                               decreaseBut->getAcquisition().height);

    if (dir == Horizontal) return {sum.x, max.y};
    return {max.x, sum.y};
}

rdr::Component* Slider::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Slider>(*this);
}

void Slider::fireChanged() { fireSignal(Event(Event::ValueChanged, value)); }

bool Slider::handleScroll(const Event& scroll) {
    if (getAcquisition().contains(scroll.mousePosition())) {
        incrementValue(-scroll.scrollDelta());
        return true;
    }
    return false;
}

void Slider::incrementValue(float incs) {
    value += incs * increment;
    valueChanged();
}

void Slider::sliderMoved(const Event& drag) {
    if (drag.type() != Event::Dragged) return;

    const float dragAmount = dir == Horizontal ? (drag.mousePosition().x - drag.dragStart().x) :
                                                 (drag.mousePosition().y - drag.dragStart().y);

    value += dragAmount / freeSpace;
    valueChanged();
}

void Slider::clicked(const Event& click) {
    if (click.type() != Event::LeftClicked) return;

    float pos = 0;
    if (dir == Horizontal) {
        pos = click.mousePosition().x - getAcquisition().left;
        pos -= decreaseBut->visible() ? decreaseBut->getAcquisition().width : 0.f;
    }
    else {
        pos = click.mousePosition().y - getAcquisition().top;
        pos -= decreaseBut->visible() ? decreaseBut->getAcquisition().height : 0.f;
    }

    value = pos / freeSpace;
    valueChanged();
}

int Slider::calculateFreeSize() const {
    int size = 0;
    if (dir == Horizontal) {
        size = getAcquisition().width - slider->getAcquisition().width;
        if (increaseBut->visible()) size -= increaseBut->getAcquisition().width;
        if (decreaseBut->visible()) size -= decreaseBut->getAcquisition().width;
    }
    else {
        size = getAcquisition().height - slider->getAcquisition().height;
        if (increaseBut->visible()) size -= increaseBut->getAcquisition().height;
        if (decreaseBut->visible()) size -= decreaseBut->getAcquisition().height;
    }
    return size;
}

void Slider::onAcquisition() {
    if (value < 0) value = 0;
    if (value > 1) value = 1;

    constexpr float ButPadding = 0.15f;
    const float butSize        = std::min(getAcquisition().width, getAcquisition().height);
    freeSpace    = (dir == Horizontal) ? getAcquisition().width : getAcquisition().height;
    float offset = 0;
    if (decreaseBut->visible()) {
        decreaseBut->setChildPadding(butSize * ButPadding, false);
        Packer::manuallyPackElement(
            decreaseBut, {getAcquisition().left, getAcquisition().top, butSize, butSize});
        offset += butSize;
        freeSpace -= butSize;
    }
    if (increaseBut->visible()) {
        increaseBut->setChildPadding(butSize * ButPadding, false);
        const float x = (dir == Horizontal) ? (getAcquisition().width - butSize) : (0);
        const float y = (dir == Vertical) ? (getAcquisition().height - butSize) : (0);

        Packer::manuallyPackElement(
            increaseBut, {x + getAcquisition().left, y + getAcquisition().top, butSize, butSize});
        freeSpace -= butSize;
    }

    sliderSize = std::floor(freeSpace * sliderSizeRatio);
    freeSpace -= sliderSize;
    const float pos = offset + freeSpace * value;

    if (dir == Horizontal)
        Packer::manuallyPackElement(
            slider, {pos + getAcquisition().left, getAcquisition().top, sliderSize, butSize});
    else
        Packer::manuallyPackElement(
            slider, {getAcquisition().left, pos + getAcquisition().top, butSize, sliderSize});
}

void Slider::updateSliderPos() {
    const float offset =
        increaseBut->visible() ? std::min(getAcquisition().width, getAcquisition().height) : 0.f;
    const float pos = offset + freeSpace * value;

    if (dir == Horizontal) { slider->setPosition(getPosition() + sf::Vector2f{pos, 0}); }
    else { slider->setPosition(getPosition() + sf::Vector2f{0, pos}); }
}

bool Slider::propagateEvent(const Event& e) { return sendEventToChildren(e); }

void Slider::constrainValue() {
    if (std::isnan(value)) value = 0.f;
    if (value < 0.f) value = 0.f;
    if (value > 1.f) value = 1.f;
}

void Slider::valueChanged() {
    constrainValue();
    updateSliderPos();
    fireChanged();
}

} // namespace gui
} // namespace bl
