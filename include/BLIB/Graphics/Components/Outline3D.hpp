#ifndef BLIB_GRAPHICS_COMPONENTS_OUTLINE3D_HPP
#define BLIB_GRAPHICS_COMPONENTS_OUTLINE3D_HPP

#include <BLIB/Components/Outline.hpp>
#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace rc
{
namespace rcom
{
struct DrawableBase;
} // namespace rcom
} // namespace rc
namespace ecs
{
class Registry;
}
namespace gfx
{
namespace bcom
{
/**
 * @brief Component that adds an outline to 3d objects when rendered
 *
 * @ingroup Graphics
 */
class Outline3D {
public:
    /**
     * @brief Creates or modifies the outline around this object
     *
     * @param color The color of the outline
     * @param thickness The thickness of the outline in world units
     */
    void showOutline(const rc::Color& color, float thickness);

    /**
     * @brief Removes the outline around this object
     *
     * @param specialization The specialization to reset the object to. Defaults to what the
     *                       specialization was prior to the outline being added
     */
    void removeOutline(std::uint32_t specialization = std::numeric_limits<std::uint32_t>::max());

    /**
     * @brief Returns the outline. Only call if an outline is present
     */
    com::Outline& getOutline() { return *outline; }

    /**
     * @brief Returns the outline. Only call if an outline is present
     */
    const com::Outline& getOutline() const { return *outline; }

    /**
     * @brief Returns whether this object has an outline or not
     */
    bool hasOutline() const { return outline != nullptr; }

    /**
     * @brief Sets a different pipeline specialization to use for the first pass. Only needed if
     *        using custom pipelines that don't use Specializations3D::OutlineMainPass
     *
     * @param specialization The specialization to use for the main pass
     */
    void setOutlineSpecialization(std::uint32_t specialization);

protected:
    /**
     * @brief Creates the outline component
     */
    Outline3D();

    /**
     * @brief Initializes the outline component
     *
     * @param registry The ECS registry
     * @param entity The entity of the object to outline
     * @param drawable The drawable component of the object to outline
     */
    void init(ecs::Registry& registry, ecs::Entity entity, rc::rcom::DrawableBase* drawable);

private:
    ecs::Entity entityId;
    ecs::Registry* registry;
    com::Outline* outline;
    rc::rcom::DrawableBase* drawable;
    std::uint32_t returnSpecialization;
    std::uint32_t outlineSpecialization;
};

} // namespace bcom
} // namespace gfx
} // namespace bl

#endif
