#include <BLIB/Interfaces/Menu/Items/ToggleTextItem.hpp>

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

void ToggleTextItem::doCreate(engine::World& world) {
    TextItem::doCreate(world);
    box.create(world, {width, width});
    innerBox.create(world, {width * InnerRatio, width * InnerRatio});
    box.setParent(getTextObject().entity());
    innerBox.setParent(box.entity());
    update();
}

void ToggleTextItem::doSceneAdd(rc::Scene* s) {
    TextItem::doSceneAdd(s);
    box.addToScene(s, rc::UpdateSpeed::Static);
    innerBox.addToScene(s, rc::UpdateSpeed::Static);
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

        box.setFillColor(fillColor);
        box.setOutlineColor(borderColor);
        box.setOutlineThickness(-borderThickness);
        box.setSize({width, width});
        box.getTransform().setOrigin({0.f, hw});
        innerBox.setFillColor(borderColor);
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

void ToggleTextItem::draw(rc::scene::CodeScene::RenderContext& ctx) {
    TextItem::draw(ctx);
    box.draw(ctx);
    innerBox.draw(ctx);
}

} // namespace menu
} // namespace bl
