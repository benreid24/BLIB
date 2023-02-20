#ifndef BLIB_RENDER_RENDERABLES_3D_MESH_HPP
#define BLIB_RENDER_RENDERABLES_3D_MESH_HPP

#include <BLIB/Render/Primitives/IndexBuffer.hpp>
#include <BLIB/Render/Renderables/Renderable.hpp>

namespace bl
{
namespace render
{
/// Renderables and utilities for 3d rendering
namespace r3d
{
class Mesh : public Renderable {
public:
    Mesh(std::uint32_t pipelineId = Config::PipelineIds::OpaqueMeshes, bool transparent = false);

    constexpr IndexBuffer& buffer();

    constexpr const IndexBuffer& buffer() const;

    void attachBuffer();

private:
    IndexBuffer indices;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr IndexBuffer& Mesh::buffer() { return indices; }

inline constexpr const IndexBuffer& Mesh::buffer() const { return indices; }

} // namespace r3d
} // namespace render
} // namespace bl

#endif
