#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONBINDINGS_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONBINDINGS_HPP

#include <BLIB/Components/MaterialInstance.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Render/Descriptors/Generic/ObjectStorageBuffer.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <BLIB/Render/Materials/MaterialId.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace ds
{
namespace priv
{
using Transform2DBinding = ObjectStorageBuffer<glm::mat4, com::Transform2D>;
using Transform3DBinding = ObjectStorageBuffer<glm::mat4, com::Transform3D>;
using TextureBinding =
    ObjectStorageBuffer<std::uint32_t, com::MaterialInstance, true, buf::StaticSSBO<std::uint32_t>,
                        buf::StaticSSBO<std::uint32_t>>;
using MaterialBinding =
    ObjectStorageBuffer<mat::MaterialId, com::MaterialInstance, true,
                        buf::StaticSSBO<std::uint32_t>, buf::StaticSSBO<std::uint32_t>>;

} // namespace priv
} // namespace ds
} // namespace rc
} // namespace bl

#endif
