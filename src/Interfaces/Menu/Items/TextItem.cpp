#include <BLIB/Interfaces/Menu/Items/TextItem.hpp>

namespace bl
{
namespace menu
{
TextItem::Ptr TextItem::create(const std::string& text, const sf::VulkanFont& font,
                               const sf::Color& color, unsigned int fontSize, std::uint32_t style) {
    return Ptr(new TextItem(text, font, color, fontSize, style));
}

TextItem::TextItem(const std::string& t, const sf::VulkanFont& font, const sf::Color& color,
                   unsigned int fontSize, std::uint32_t style)
: string(t)
, font(font)
, color(color)
, fontSize(fontSize)
, style(style) {}

void TextItem::doCreate(engine::World& world) {
    text.create(world, font, string, fontSize, color, style);
}

void TextItem::doSceneAdd(rc::Scene* s) { text.addToScene(s, rc::UpdateSpeed::Static); }

void TextItem::doSceneRemove() { text.removeFromScene(); }

ecs::Entity TextItem::getEntity() const { return text.entity(); }

gfx::Text& TextItem::getTextObject() { return text; }

glm::vec2 TextItem::getSize() const { return text.getGlobalSize(); }

void TextItem::draw(rc::scene::CodeScene::RenderContext& ctx) { text.draw(ctx); }

} // namespace menu
} // namespace bl
