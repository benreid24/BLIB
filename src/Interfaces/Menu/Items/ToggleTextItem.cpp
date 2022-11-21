#include <BLIB/Interfaces/Menu/Items/ToggleTextItem.hpp>

namespace bl
{
namespace menu
{
ToggleTextItem::Ptr ToggleTextItem::create(const std::string& text, const sf::Font& font,
                                           const sf::Color& color, unsigned int fontSize) {
    return Ptr{new ToggleTextItem(text, font, color, fontSize)};
}

ToggleTextItem::ToggleTextItem(const std::string& text, const sf::Font& font,
                               const sf::Color& color, unsigned int fontSize)
: TextItem(text, font, color, fontSize)
, checked(false) {
    const float fs = static_cast<float>(fontSize);
    setBoxProperties(sf::Color(~color.toInteger()), color, fs, 2.f, fs * 0.5f, false);
    getSignal(Activated).willAlwaysCall([this]() { setChecked(!checked); });
}

bool ToggleTextItem::isChecked() const { return checked; }

void ToggleTextItem::setChecked(bool c) { checked = c; }

void ToggleTextItem::setBoxProperties(sf::Color fillColor, sf::Color borderColor, float width,
                                      float borderThickness, float pad, bool showOnLeft) {
    const float hw = width * 0.5f;
    padding        = pad;
    leftSide       = showOnLeft;

    box.setFillColor(fillColor);
    box.setOutlineColor(borderColor);
    box.setOutlineThickness(-borderThickness);
    box.setSize({width, width});
    box.setOrigin(0.f, hw);
    innerBox.setFillColor(borderColor);
    innerBox.setSize({width * 0.45f, width * 0.45f});
    innerBox.setOrigin(innerBox.getSize() * 0.5f);

    const sf::Vector2f ns = getSize();
    const sf::Vector2f ts = TextItem::getSize();
    box.setPosition(leftSide ? 0.f : ts.x + padding, ns.y * 0.6f);
    innerBox.setPosition(box.getPosition() + sf::Vector2f(hw, 0.f));
    textOffset = leftSide ? width + padding : 0.f;
}

sf::Vector2f ToggleTextItem::getSize() const {
    const sf::Vector2f ts  = TextItem::getSize();
    const sf::Vector2f& bs = box.getSize();
    return {ts.x + padding + bs.x, std::max(ts.y, bs.y)};
}

void ToggleTextItem::render(sf::RenderTarget& target, sf::RenderStates states,
                            const sf::Vector2f& position) const {
    states.transform.translate(position);
    target.draw(box, states);
    if (checked) { target.draw(innerBox, states); }
    states.transform.translate(textOffset, 0.f);
    TextItem::render(target, states, {});
}

} // namespace menu
} // namespace bl
