#include <BLIB/GUI/Elements/Slider.hpp>

namespace bl
{
namespace gui
{
Slider::Ptr Slider::create(Direction dir, const std::string& group, const std::string& id) {
    return Ptr(new Slider(dir, group, id));
}

Slider::Slider(Direction d, const std::string& g, const std::string& i)
: Element(g, i)
, dir(d) {
    // TODO - init elements
}

float Slider::getValue() const { return value; }

void Slider::setValue(float v) {
    value = v;
    packElements();
}

void Slider::setSliderSize(float s) {
    buttonSize = s;
    packElements();
}

void Slider::setSliderIncrement(float i) { increment = i; }

Button::Ptr Slider::getSlider() { return slider; }

Button::Ptr Slider::getIncreaseButton() { return increaseBut; }

Button::Ptr Slider::getDecreaseButton() { return decreaseBut; }

sf::Vector2i Slider::minimumRequisition() const {
    const sf::Vector2i max(
        std::max(std::max(slider->getAcquisition().width, increaseBut->getAcquisition().width),
                 decreaseBut->getAcquisition().width),
        std::max(
            std::max(slider->getAcquisition().height, increaseBut->getAcquisition().height),
            decreaseBut->getAcquisition().height));
    const sf::Vector2i sum(
        slider->getAcquisition().width + increaseBut->getAcquisition().width +
            decreaseBut->getAcquisition().width,
        slider->getAcquisition().height + increaseBut->getAcquisition().height +
            decreaseBut->getAcquisition().height);

    if (dir == Horizontal) return {sum.x, max.y};
    return {max.x, sum.y};
}

bool Slider::handleRawEvent(const RawEvent& event) {
    return slider->handleEvent(event) || increaseBut->handleEvent(event) ||
           decreaseBut->handleEvent(event);
}

void Slider::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      Renderer::Ptr renderer) const {
    // TODO - render
}

void Slider::increaseClicked() {
    value += increment;
    packElements();
}

void Slider::decreaseClicked() {
    value -= increment;
    packElements();
}

void Slider::sliderMoved(const Action& drag) {
    if (drag.type != Action::Dragged) return;

    const unsigned int size = calculateFreeSize();
    int dragAmount          = 0;
    if (dir == Horizontal)
        dragAmount = drag.position.x - drag.data.dragStart.x;
    else
        dragAmount = drag.position.y - drag.data.dragStart.y;

    const float d = static_cast<float>(dragAmount) / static_cast<float>(size);
    value += d;
    packElements();
}

unsigned int Slider::calculateFreeSize() const {
    unsigned int size = 0;
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

    // TODO - pack
}

} // namespace gui
} // namespace bl