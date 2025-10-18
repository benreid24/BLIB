#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONBINDINGS_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_COMMONBINDINGS_HPP

#include <BLIB/Components/MaterialInstance.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Render/Descriptors/Builtin/Transform3DPayload.hpp>
#include <BLIB/Render/Descriptors/Generic/ObjectBufferBinding.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <BLIB/Render/Materials/MaterialId.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace sri
{
using Transform2DBuffer = sr::EntityComponentShaderResource<com::Transform2D, glm::mat4>;
constexpr sr::Key<Transform2DBuffer> Transform2DBufferKey{"__builtin__Transform2DBuffer"};

using TextureIdBuffer = sr::EntityComponentShaderResource<com::MaterialInstance, std::uint32_t,
                                                          buf::StaticSSBO<std::uint32_t>,
                                                          buf::StaticSSBO<std::uint32_t>>;
constexpr sr::Key<TextureIdBuffer> TextureIdBufferKey{"__builtin_TextureIdBuffer"};

using Transform3DBuffer =
    sr::EntityComponentShaderResource<com::Transform3D, dsi::Transform3DPayload>;
constexpr sr::Key<Transform3DBuffer> Transform3DBufferKey{"__builtin__Transform3DBuffer"};

using MaterialIdBuffer = sr::EntityComponentShaderResource<com::MaterialInstance, mat::MaterialId,
                                                           buf::StaticSSBO<mat::MaterialId>,
                                                           buf::StaticSSBO<mat::MaterialId>>;
constexpr sr::Key<MaterialIdBuffer> MaterialIdBufferKey{"__builtin_MaterialIdBuffer"};

} // namespace sri

/// Built-in descriptor sets
namespace dsi
{
namespace priv
{
using Transform2DBinding =
    ds::ObjectBufferBinding<sri::Transform2DBuffer, sri::Transform2DBufferKey>;

using Transform3DBinding =
    ds::ObjectBufferBinding<sri::Transform3DBuffer, sri::Transform3DBufferKey>;

using TextureBinding = ds::ObjectBufferBinding<sri::TextureIdBuffer, sri::TextureIdBufferKey,
                                               VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, true>;

using MaterialBinding = ds::ObjectBufferBinding<sri::MaterialIdBuffer, sri::MaterialIdBufferKey,
                                                VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, true>;

} // namespace priv
} // namespace dsi
} // namespace rc
} // namespace bl

#endif
