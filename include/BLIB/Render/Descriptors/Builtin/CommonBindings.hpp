#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONBINDINGS_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONBINDINGS_HPP

#include <BLIB/Components/MaterialInstance.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Render/Descriptors/Builtin/Transform3DPayload.hpp>
#include <BLIB/Render/Descriptors/Generic/ObjectStorageBuffer.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <BLIB/Render/Materials/MaterialId.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
class Renderer;

/// Built-in descriptor sets
namespace dsi
{
namespace priv
{
using Transform2DBinding = ds::ObjectStorageBuffer<glm::mat4, com::Transform2D>;
using Transform3DBinding = ds::ObjectStorageBuffer<Transform3DPayload, com::Transform3D>;
using TextureBinding =
    ds::ObjectStorageBuffer<std::uint32_t, com::MaterialInstance, true,
                            buf::StaticSSBO<std::uint32_t>, buf::StaticSSBO<std::uint32_t>>;
using MaterialBinding =
    ds::ObjectStorageBuffer<mat::MaterialId, com::MaterialInstance, true,
                            buf::StaticSSBO<mat::MaterialId>, buf::StaticSSBO<mat::MaterialId>>;

} // namespace priv
} // namespace dsi
} // namespace rc
} // namespace bl

#endif
