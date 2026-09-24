#ifndef BLIB_GRAPHICS_RECTANGULARPRISM_HPP
#define BLIB_GRAPHICS_RECTANGULARPRISM_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Graphics/Components/Outline3D.hpp>
#include <BLIB/Graphics/Components/Transform3D.hpp>
#include <BLIB/Graphics/Drawable.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Basic drawable for rectangular prisms
 *
 * @ingroup Graphics
 */
class RectangularPrism
: public Drawable<com::BasicMesh>
, public bcom::Transform3D
, public bcom::Outline3D {
public:
    /**
     * @brief Does nothing
     */
    RectangularPrism();

    /**
     * @brief Creates the rectangular prism with a given size and material
     *
     * @param world The world to create the rectangular prism in
     * @param length The length of the rectangular prism in world coordinates
     * @param crossSection The width and height of the rectangular prism in world coordinates
     * @param material The material to assign to the rectangular prism
     * @param materialPipelineId The id of the material pipeline to render with
     */
    RectangularPrism(engine::World& world, float length, const glm::vec2& crossSection,
                     const bl::rc::res::MaterialRef& material = {},
                     std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Creates the cube with a given size and material
     *
     * @param world The world to create the cube in
     * @param length The length of the rectangular prism in world coordinates
     * @param crossSection The width and height of the rectangular prism in world coordinates
     * @param material The material to assign to the cube
     * @param materialPipelineId The id of the material pipeline to render with
     */
    void create(engine::World& world, float length, const glm::vec2& crossSection,
                const bl::rc::res::MaterialRef& material = {},
                std::uint32_t materialPipelineId         = rc::cfg::MaterialPipelineIds::Mesh3D);

private:
    virtual void scaleToSize(const glm::vec2& size) override;
};

} // namespace gfx
} // namespace bl

#endif
