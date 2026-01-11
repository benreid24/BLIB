#include <BLIB/Graphics/Components/Outline3D.hpp>

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Config/Specializations3D.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
Outline3D::Outline3D()
: entityId(ecs::InvalidEntity)
, registry(nullptr)
, outline(nullptr)
, drawable(nullptr)
, returnSpecialization(0)
, outlineSpecialization(rc::cfg::Specializations3D::OutlineMainPass) {}

void Outline3D::init(ecs::Registry& r, ecs::Entity e, rc::rcom::DrawableBase* d) {
    entityId = e;
    registry = &r;
    drawable = d;
}

void Outline3D::showOutline(const rc::Color& color, float thickness) {
    if (outline) {
        outline->setColor(color);
        outline->setThickness(thickness);
    }
    else { outline = registry->emplaceComponent<com::Outline>(entityId, color, thickness); }
    returnSpecialization = drawable->getPipelineSpecialization();
    drawable->setPipelineSpecialization(outlineSpecialization);
}

void Outline3D::removeOutline(std::uint32_t spec) {
    if (registry && outline) {
        registry->removeComponent<com::Outline>(entityId);
        outline = nullptr;
    }
    if (drawable) {
        drawable->setPipelineSpecialization(
            spec != std::numeric_limits<std::uint32_t>::max() ? spec : returnSpecialization);
    }
}

void Outline3D::setOutlineSpecialization(std::uint32_t specialization) {
    outlineSpecialization = specialization;
    if (drawable && outline) { drawable->setPipelineSpecialization(specialization); }
}

} // namespace bcom
} // namespace gfx
} // namespace bl
