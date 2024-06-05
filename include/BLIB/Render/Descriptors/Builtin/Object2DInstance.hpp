#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DINSTANCE_HPP

#include <BLIB/Components/Texture.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Render/Descriptors/Generic/ObjectStorageBuffer.hpp>
#include <BLIB/Render/Descriptors/GenericDescriptorSetInstance.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class Renderer;

namespace ds
{
namespace priv
{
using Transform2DBinding = ObjectStorageBuffer<glm::mat4, com::Transform2D>;
using TextureBinding =
    ObjectStorageBuffer<std::uint32_t, com::Texture, buf::StaticSSBO<std::uint32_t>,
                        buf::StaticSSBO<std::uint32_t>>;
using Object2DBindings = Bindings<Transform2DBinding, TextureBinding>;
} // namespace priv

/**
 * @brief Descriptor set instance used by all 2d objects in the engine default pipelines. Contains
 *        the object transform matrix and texture id
 *
 * @ingroup Renderer
 */
using Object2DInstance = GenericDescriptorSetInstance<priv::Object2DBindings>;

} // namespace ds
} // namespace rc
} // namespace bl

#endif
