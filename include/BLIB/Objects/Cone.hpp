#ifndef BLIB_OBJECTS_CONE_HPP
#define BLIB_OBJECTS_CONE_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Objects/Components/Outline3D.hpp>
#include <BLIB/Objects/Components/Transform3D.hpp>
#include <BLIB/Objects/Drawable.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace obj
{
/**
 * @brief Basic drawable for 3d cones
 *
 * @ingroup Objects
 */
class Cone
: public Drawable<com::BasicMesh>
, public bcom::Transform3D
, public bcom::Outline3D {
public:
    /**
     * @brief Does nothing
     */
    Cone();

    /**
     * @brief Creates the cone with a given size and material
     *
     * @param world The world to create the cone in
     * @param height The height of the cone
     * @param radius The radius of the cone in world coordinates
     * @param pointCount The number of points to make the bottom of the cone with
     * @param material The material to assign to the cone
     * @param materialPipelineId The id of the material pipeline to render with
     */
    Cone(engine::World& world, float height, float radius, unsigned int pointCount,
         const bl::rc::res::MaterialRef& material = {},
         std::uint32_t materialPipelineId         = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Creates the cone with a given size and material
     *
     * @param world The world to create the cone in
     * @param height The height of the cone
     * @param radius The radius of the cone in world coordinates
     * @param pointCount The number of points to make the bottom of the cone with
     * @param material The material to assign to the cone
     * @param materialPipelineId The id of the material pipeline to render with
     */
    void create(engine::World& world, float height, float radius, unsigned int pointCount,
                const bl::rc::res::MaterialRef& material = {},
                std::uint32_t materialPipelineId         = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Sets the color of the cone
     *
     * @param color The color to assign to all the vertices
     */
    void setColor(const rc::Color& color);

private:
    virtual void scaleToSize(const glm::vec2& size) override;
};

} // namespace obj
} // namespace bl

#endif
