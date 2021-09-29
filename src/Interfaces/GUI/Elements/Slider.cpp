#include <BLIB/Interfaces/GUI/Elements/Slider.hpp>

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
Slider::Ptr Slider::create(Direction dir) { return Ptr(new Slider(dir)); }

Slider::Slider(Direction d)
: Element()
, dir(d)
, buttonSize(0.2)
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
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Slider::packElements, this));
    getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Slider::clicked, this, _1));
    slider->getSignal(Event::Dragged).willAlwaysCall(std::bind(&Slider::sliderMoved, this, _1));
    increaseBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&Slider::incrementValue, this, 1));
    decreaseBut->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&Slider::incrementValue, this, -1));
}

float Slider::getValue() const { return value; }

void Slider::setValue(float v) {
    value = v;
    fireChanged();
    packElements();
}

void Slider::setSliderSize(float s) {
    buttonSize = s;
    if (buttonSize <= 0)
        buttonSize = 0.01;
    else if (buttonSize > 1)
        buttonSize = 1;
    packElements();
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
    packElements();
    fireChanged();
}

void Slider::sliderMoved(const Event& drag) {
    if (drag.type() != Event::Dragged) return;

    const int size = calculateFreeSize();
    if (size == 0) return;

    int dragAmount = 0;
    int pos        = 0;
    int offset     = 0;
    if (dir == Horizontal) {
        pos        = slider->getAcquisition().left;
        dragAmount = drag.mousePosition().x - drag.dragStart().x;
        offset     = decreaseBut->visible() ? decreaseBut->getAcquisition().width : 0;
    }
    else {
        pos        = slider->getAcquisition().top;
        dragAmount = drag.mousePosition().y - drag.dragStart().y;
        offset     = decreaseBut->visible() ? decreaseBut->getAcquisition().height : 0;
    }

    pos += dragAmount - offset;
    if (pos < 0) pos = 0;
    if (pos > size) pos = size;

    value = static_cast<float>(pos) / static_cast<float>(size);
    if (dir == Horizontal)
        slider->setPosition({pos + offset + getAcquisition().left,
                             slider->getAcquisition().top + getAcquisition().top});
    else
        slider->setPosition({slider->getAcquisition().left + getAcquisition().left,
                             pos + offset + getAcquisition().top});
    fireChanged();
}

void Slider::clicked(const Event& click) {
    if (click.type() != Event::LeftClicked) return;

    const float size = calculateFreeSize();
    float pos        = 0;
    float offset     = 0;
    if (dir == Horizontal) {
        pos    = click.mousePosition().x - getAcquisition().left;
        offset = decreaseBut->visible() ? decreaseBut->getAcquisition().width : 0;
    }
    else {
        pos    = click.mousePosition().y - getAcquisition().top;
        offset = decreaseBut->visible() ? decreaseBut->getAcquisition().height : 0;
    }

    pos -= offset;
    if (pos < 0) pos = 0;
    if (pos > size) pos = size;

    value = pos / size;
    if (dir == Horizontal)
        slider->setPosition({pos + offset, slider->getAcquisition().top});
    else
        slider->setPosition({slider->getAcquisition().left, pos + offset});
    fireChanged();
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

void Slider::packElements() {
    if (value < 0) value = 0;
    if (value > 1) value = 1;

    const float butSize = std::min(getAcquisition().width, getAcquisition().height);
    float space         = (dir == Horizontal) ? getAcquisition().width : getAcquisition().height;
    float offset        = 0;
    if (decreaseBut->visible()) {
        Packer::manuallyPackElement(
            decreaseBut, {getAcquisition().left, getAcquisition().top, butSize, butSize});
        offset += butSize;
        space -= butSize;
    }
    if (increaseBut->visible()) {
        const float x = (dir == Horizontal) ? (getAcquisition().width - butSize) : (0);
        const float y = (dir == Vertical) ? (getAcquisition().height - butSize) : (0);

        Packer::manuallyPackElement(
            increaseBut, {x + getAcquisition().left, y + getAcquisition().top, butSize, butSize});
        space -= butSize;
    }

    const float sliderSize = std::floor(space * buttonSize);
    const float pos        = offset + std::floor((space - sliderSize) * value);

    if (dir == Horizontal)
        Packer::manuallyPackElement(
            slider, {pos + getAcquisition().left, getAcquisition().top, sliderSize, butSize});
    else
        Packer::manuallyPackElement(
            slider, {getAcquisition().left, pos + getAcquisition().top, butSize, sliderSize});
}

} // namespace gui
} // namespace bl
