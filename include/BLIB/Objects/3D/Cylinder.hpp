#ifndef BLIB_OBJECTS_3D_CYLIDNER_HPP
#define BLIB_OBJECTS_3D_CYLINDER_HPP

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
 * @brief Basic drawable for a cylidner
 *
 * @ingroup Objects
 */
class Cylinder : public DrawableObject3D<com::BasicMesh> {
public:
    /**
     * @brief Does nothing
     */
    Cylinder();

    /**
     * @brief Creates the cylinder with a given size and material
     *
     * @param world The world to create the cylinder in
     * @param height The height of the cylinder
     * @param radius The radius of the cylinder in world coordinates
     * @param pointCount The number of points to make the bottom of the cylinder with
     * @param material The material to assign to the cylinder
     * @param materialPipelineId The id of the material pipeline to render with
     */
    Cylinder(engine::World& world, float height, float radius, unsigned int pointCount,
             const bl::rc::res::MaterialRef& material = {},
             std::uint32_t materialPipelineId         = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Creates the cylinder with a given size and material
     *
     * @param world The world to create the cylinder in
     * @param height The height of the cylinder
     * @param radius The radius of the cylinder in world coordinates
     * @param pointCount The number of points to make the bottom of the cylinder with
     * @param material The material to assign to the cylinder
     * @param materialPipelineId The id of the material pipeline to render with
     */
    void create(engine::World& world, float height, float radius, unsigned int pointCount,
                const bl::rc::res::MaterialRef& material = {},
                std::uint32_t materialPipelineId         = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Sets the color of the cylinder
     *
     * @param color The color to assign to all the vertices
     */
    void setColor(const rc::Color& color);
};

} // namespace o3d
} // namespace obj
} // namespace bl

#endif
