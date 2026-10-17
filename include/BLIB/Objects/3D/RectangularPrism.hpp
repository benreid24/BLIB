#ifndef BLIB_OBJECTS_3D_RECTANGULARPRISM_HPP
#define BLIB_OBJECTS_3D_RECTANGULARPRISM_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Objects/3D/DrawableObject3D.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace obj
{
namespace o3d
{
/**
 * @brief Basic drawable for rectangular prisms
 *
 * @ingroup Objects
 */
class RectangularPrism : public DrawableObject3D<com::BasicMesh> {
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

    /**
     * @brief Sets the color of the rectangular prism
     *
     * @param color The color to assign to all the vertices
     */
    void setColor(const rc::Color& color);
};

} // namespace o3d
} // namespace obj
} // namespace bl

#endif
