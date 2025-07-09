#ifndef BLIB_GRAPHICS_SPHERE_HPP
#define BLIB_GRAPHICS_SPHERE_HPP

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
 * @brief Basic drawable for 3d spheres
 *
 * @ingroup Graphics
 */
class Sphere
: public Drawable<com::BasicMesh>
, public bcom::Transform3D
, public bcom::Outline3D {
public:
    /**
     * @brief Does nothing
     */
    Sphere();

    /**
     * @brief Creates the sphere with a given size and material
     *
     * @param world The world to create the sphere in
     * @param radius The radius of the sphere in world coordinates
     * @param detailLevel How detailed to make the sphere. Triangle count = detail * 20
     * @param material The material to assign to the sphere
     * @param materialPipelineId The id of the material pipeline to render with
     */
    Sphere(engine::World& world, float radius, int detailLevel,
           const bl::rc::res::MaterialRef& material = {},
           std::uint32_t materialPipelineId         = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Creates the sphere with a given size and material
     *
     * @param world The world to create the sphere in
     * @param radius The radius of the sphere in world coordinates
     * @param detailLevel How detailed to make the sphere. Triangle count = detail * 20
     * @param material The material to assign to the sphere
     * @param materialPipelineId The id of the material pipeline to render with
     */
    void create(engine::World& world, float radius, int detailLevel,
                const bl::rc::res::MaterialRef& material = {},
                std::uint32_t materialPipelineId         = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Populates the given vectors with vertices and indices to make a sphere
     *
     * @param radius The radius of the sphere
     * @param detailLevel How detailed to make the sphere. Triangle count = detail * 20
     * @param outVertices The output vertices container
     * @param outIndices The output indices container
     */
    static void makeSphere(float radius, int detailLevel,
                           std::vector<rc::prim::Vertex3D>& outVertices,
                           std::vector<std::uint32_t>& outIndices);

    /**
     * @brief Sets the color of the sphere
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
