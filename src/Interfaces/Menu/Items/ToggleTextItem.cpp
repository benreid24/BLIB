#include <BLIB/Interfaces/Menu/Items/ToggleTextItem.hpp>

#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace menu
{
namespace
{
constexpr float InnerRatio = 0.45f;
}

ToggleTextItem::Ptr ToggleTextItem::create(const std::string& text, const sf::VulkanFont& font,
                                           const sf::Color& color, unsigned int fontSize) {
    return Ptr{new ToggleTextItem(text, font, color, fontSize)};
}

ToggleTextItem::ToggleTextItem(const std::string& text, const sf::VulkanFont& font,
                               const sf::Color& color, unsigned int fontSize)
: TextItem(text, font, color, fontSize)
, checked(false) {
    const float fs = static_cast<float>(fontSize);
    setBoxProperties(sf::Color(~color.toInteger()), color, fs, 2.f, fs * 0.5f, false);
    getSignal(Activated).willAlwaysCall([this]() { setChecked(!checked); });
}

com::Transform2D& ToggleTextItem::doCreate(engine::Engine& engine, ecs::Entity parent) {
    com::Transform2D& r = TextItem::doCreate(engine, parent);
    box.create(engine, {width, width});
    innerBox.create(engine, {width * InnerRatio, width * InnerRatio});
    box.setParent(getTextObject().entity());
    innerBox.setParent(box.entity());
    update();
    return r;
}

void ToggleTextItem::doSceneAdd(rc::Overlay* overlay) {
    TextItem::doSceneAdd(overlay);
    box.addToScene(overlay, rc::UpdateSpeed::Static);
    innerBox.addToScene(overlay, rc::UpdateSpeed::Static);
    innerBox.setHidden(!checked);
}

void ToggleTextItem::doSceneRemove() {
    TextItem::doSceneRemove();
    box.removeFromScene();
    innerBox.removeFromScene();
}

ecs::Entity ToggleTextItem::getEntity() const { return TextItem::getEntity(); }

void ToggleTextItem::update() {
    if (getTextObject().entity() != ecs::InvalidEntity) {
        const float hw = width * 0.5f;

        box.setFillColor(sfcol(fillColor));
        box.setOutlineColor(sfcol(borderColor));
        box.setOutlineThickness(-borderThickness);
        box.setSize({width, width});
        box.getTransform().setOrigin({0.f, hw});
        innerBox.setFillColor(sfcol(borderColor));
        innerBox.setSize({width * InnerRatio, width * InnerRatio});
        innerBox.getTransform().setOrigin(innerBox.getSize() * 0.5f);

        const glm::vec2 ns = getSize();
        const glm::vec2 ts = TextItem::getSize();
        box.getTransform().setPosition({leftSide ? 0.f : ts.x + padding, ns.y * 0.6f});
        innerBox.getTransform().setPosition({hw, 0.f});

        // use origin to offset text without moving children
        getTextObject().getTransform().setOrigin({leftSide ? width + padding : 0.f, 0.f});

        innerBox.setHidden(!checked);
    }
}

bool ToggleTextItem::isChecked() const { return checked; }

void ToggleTextItem::setChecked(bool c) {
    checked = c;
    if (innerBox.entity() != ecs::InvalidEntity) { innerBox.setHidden(!checked); }
}

void ToggleTextItem::setBoxProperties(sf::Color fc, sf::Color bc, float w, float bt, float pad,
                                      bool showOnLeft) {
    fillColor       = fc;
    borderColor     = bc;
    width           = w;
    borderThickness = bt;
    padding         = pad;
    leftSide        = showOnLeft;
    update();
}

glm::vec2 ToggleTextItem::getSize() const {
    const glm::vec2 ts  = TextItem::getSize();
    const glm::vec2& bs = box.getSize();
    return {ts.x + padding + bs.x, std::max(ts.y, bs.y)};
}

} // namespace menu
} // namespace bl
