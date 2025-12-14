#ifndef BLIB_RENDER_SHADERASSETS_SHADERASSET_HPP
#define BLIB_RENDER_SHADERASSETS_SHADERASSET_HPP

#include <BLIB/Render/Scenes/Key.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class Scene;
class RenderTarget;

namespace vk
{
struct VulkanState;
}

/// Module containing shader resources like images and buffers
namespace sr
{
/**
 * @brief Base class for shader inputs. Shader inputs are the images and buffers that descriptors
 *        point to. Descriptor sets share shader inputs for common data via the ShaderResourceStore
 *
 * @ingroup Renderer
 */
class ShaderResource {
public:
    /**
     * @brief Destroys the shader input
     */
    virtual ~ShaderResource() = default;

    /**
     * @brief Derived classes should allocate resources here
     *
     * @param engine The game engine instance
     * @param owner The render target that owns the resource
     */
    virtual void init(engine::Engine& engine, RenderTarget& owner) = 0;

    /**
     * @brief Derived classes should release resources here
     */
    virtual void cleanup() = 0;

    /**
     * @brief Derived classes should copy local buffers to the GPU here
     */
    virtual void performTransfer() = 0;

    /**
     * @brief Derived classes should copy source data in to local buffers here
     */
    virtual void copyFromSource() = 0;

    /**
     * @brief Returns true if the dynamic descriptor sets need to be updated this frame
     */
    virtual bool dynamicDescriptorUpdateRequired() const = 0;

    /**
     * @brief Returns true if the static descriptor sets need to be updated this frame
     */
    virtual bool staticDescriptorUpdateRequired() const = 0;

    /**
     * @brief Called when an object is allocated in a scene. Provides the hook for custom resources
     *        to interact with entities and their components
     *
     * @param entity The entity being allocated
     * @param key The scene key of the entity
     * @return True if the entity could be added to this resource, false otherwise
     */
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) {
        (void)entity;
        (void)key;
        return true;
    }

    /**
     * @brief Called when an object is removed from the scene
     *
     * @param entity The entity being removed
     * @param key The scene key of the entity
     */
    virtual void releaseObject(ecs::Entity entity, scene::Key key) {
        (void)entity;
        (void)key;
    }
};

} // namespace sr
} // namespace rc
} // namespace bl

#endif
