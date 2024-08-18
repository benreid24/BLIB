#include <BLIB/Interfaces/Menu/Selectors/ArrowSelector.hpp>

#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace menu
{
namespace
{
const float HeightRatio = 0.5f;

std::array<glm::vec2, 3> makeTriangle(float w) {
    return std::array<glm::vec2, 3>({glm::vec2{0.f, 0.f},
                                     glm::vec2{w, w * HeightRatio},
                                     glm::vec2{0.f, 2.f * w * HeightRatio}});
}
} // namespace

ArrowSelector::Ptr ArrowSelector::create(float w, const sf::Color& f) {
    return Ptr(new ArrowSelector(w, f));
}

void ArrowSelector::notifySelection(ecs::Entity, sf::FloatRect itemArea) {
    triangle.getTransform().setPosition(
        {itemArea.left - width - 2.f,
         itemArea.top + itemArea.height * 0.5f - width * HeightRatio * 0.5f});
}

void ArrowSelector::doSceneAdd(rc::Scene* s) { triangle.addToScene(s, rc::UpdateSpeed::Static); }

void ArrowSelector::doSceneRemove() { triangle.removeFromScene(); }

void ArrowSelector::draw(rc::scene::CodeScene::RenderContext& ctx) { triangle.draw(ctx); }

ArrowSelector::ArrowSelector(float w, const sf::Color& f)
: width(w)
, fillColor(f)
, triangle() {}

gfx::Triangle& ArrowSelector::getArrow() { return triangle; }

void ArrowSelector::doCreate(engine::Engine& engine, ecs::Entity parent) {
    triangle.create(engine, makeTriangle(width));
    triangle.setFillColor(sfcol(fillColor));
    triangle.setParent(parent);
}

} // namespace menu
} // namespace bl
