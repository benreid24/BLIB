#ifndef BLIB_GRAPHICS_CYLIDNER_HPP
#define BLIB_GRAPHICS_CYLINDER_HPP

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
 * @brief Basic drawable for a cylidner
 *
 * @ingroup Graphics
 */
class Cylinder
: public Drawable<com::BasicMesh>
, public bcom::Transform3D
, public bcom::Outline3D {
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

private:
    virtual void scaleToSize(const glm::vec2& size) override;
};

} // namespace gfx
} // namespace bl

#endif
