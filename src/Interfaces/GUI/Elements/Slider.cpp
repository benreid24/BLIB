#include <BLIB/Interfaces/GUI/Elements/Slider.hpp>

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
Slider::Ptr Slider::create(Direction dir) { return Ptr(new Slider(dir)); }

Slider::Slider(Direction d)
: CompositeElement<3>()
, dir(d)
, sliderSizeRatio(0.2)
, value(0)
, increment(0.1)
, increaseImg(Canvas::create(64, 64))
, increaseBut(Button::create(increaseImg))
, decreaseImg(Canvas::create(64, 64))
, decreaseBut(Button::create(decreaseImg))
, slider(Button::create(""))
, renderedButs(false) {
    increaseBut->setExpandsHeight(true);
    increaseBut->setExpandsWidth(true);
    decreaseBut->setExpandsHeight(true);
    decreaseBut->setExpandsWidth(true);
    slider->setExpandsHeight(true);
    slider->setExpandsWidth(true);

    increaseImg->setFillAcquisition(true, false);
    decreaseImg->setFillAcquisition(true, false);

    using namespace std::placeholders;
    getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Slider::clicked, this, _1));
    slider->getSignal(Event::Dragged).willAlwaysCall(std::bind(&Slider::sliderMoved, this, _1));
    increaseBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&Slider::incrementValue, this, 1));
    decreaseBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&Slider::incrementValue, this, -1));

    Element* children[3] = {increaseBut.get(), decreaseBut.get(), slider.get()};
    registerChildren(children);
}

float Slider::getValue() const { return value; }

void Slider::setValue(float v) {
    value = v;
    valueChanged();
}

void Slider::setSliderSize(float s) {
    sliderSizeRatio = s;
    if (sliderSizeRatio <= 0)
        sliderSizeRatio = 0.01;
    else if (sliderSizeRatio > 1)
        sliderSizeRatio = 1;
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

void Slider::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      const Renderer& renderer) const {
    if (!renderedButs) {
        renderedButs = true;
        renderer.renderSliderButton(increaseImg->getTexture(), dir == Horizontal, true);
        renderer.renderSliderButton(decreaseImg->getTexture(), dir == Horizontal, false);
    }
    renderer.renderSlider(target, states, *this);
    slider->render(target, states, renderer);
    increaseBut->render(target, states, renderer);
    decreaseBut->render(target, states, renderer);
}

void Slider::fireChanged() { fireSignal(Event(Event::ValueChanged, value)); }

bool Slider::handleScroll(const Event& scroll) {
    incrementValue(-scroll.scrollDelta());
    return true;
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

    const float butSize = std::min(getAcquisition().width, getAcquisition().height);
    freeSpace           = (dir == Horizontal) ? getAcquisition().width : getAcquisition().height;
    float offset        = 0;
    if (decreaseBut->visible()) {
        Packer::manuallyPackElement(
            decreaseBut, {getAcquisition().left, getAcquisition().top, butSize, butSize});
        offset += butSize;
        freeSpace -= butSize;
    }
    if (increaseBut->visible()) {
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
    else {
        slider->setPosition(getPosition() + sf::Vector2f{0, pos});
    }
}

bool Slider::propagateEvent(const Event& e) { return sendEventToChildren(e); }

void Slider::constrainValue() {
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
