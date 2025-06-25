#ifndef BLIB_GRAPHICS_CUBE_HPP
#define BLIB_GRAPHICS_CUBE_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Graphics/Components/Transform3D.hpp>
#include <BLIB/Graphics/Drawable.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Basic drawable for 3d cubes
 *
 * @ingroup Graphics
 */
class Cube
: public Drawable<com::BasicMesh>
, public bcom::Transform3D {
public:
    /**
     * @brief Does nothing
     */
    Cube();

    /**
     * @brief Creates the cube with a given size and material
     *
     * @param world The world to create the cube in
     * @param size The size of the edges of the cube in world coordinates
     * @param material The material to assign to the cube
     * @param materialPipelineId The id of the material pipeline to render with
     */
    Cube(engine::World& world, float size, const bl::rc::res::MaterialRef& material = {},
         std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3D);

    /**
     * @brief Creates the cube with a given size and material
     *
     * @param world The world to create the cube in
     * @param size The size of the edges of the cube in world coordinates
     * @param material The material to assign to the cube
     * @param materialPipelineId The id of the material pipeline to render with
     */
    void create(engine::World& world, float size, const bl::rc::res::MaterialRef& material = {},
                std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3D);

private:
    virtual void scaleToSize(const glm::vec2& size) override;
};

} // namespace gfx
} // namespace bl

#endif
