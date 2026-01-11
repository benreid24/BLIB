#ifndef BLIB_RENDER_SHADERRESOURCES_SKELETALBONESRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_SKELETALBONESRESOURCE_HPP

#include <BLIB/Render/Buffers/BufferDoubleHostVisibleSourced.hpp>
#include <BLIB/Render/ShaderResources/BufferShaderResource.hpp>
#include <BLIB/Util/RangeAllocatorUnbounded.hpp>

namespace bl
{
namespace com
{
class Skeleton;
}
namespace ecs
{
class Registry;
}

namespace rc
{
namespace sri
{
/**
 * @brief Shader resource for the pool of bones for skeletal animation
 *
 * @ingroup Renderer
 */
class SkeletalBonesResource
: public sr::BufferShaderResource<buf::BufferDoubleHostVisibleSourced<glm::mat4>, 512> {
public:
    /**
     * @brief Creates the shader resource
     */
    SkeletalBonesResource();

    /**
     * @brief Destroys the shader resource
     */
    virtual ~SkeletalBonesResource() = default;

    /**
     * @brief Context struct to link a component to the skeletal bones resource
     *
     * @ingroup Renderer
     */
    class ComponentLink {
    public:
        /**
         * @brief Creates the link in an unlinked state
         */
        ComponentLink()
        : resource(nullptr)
        , offset(0)
        , len(0) {}

        /**
         * @brief Returns the base pointer to write bone data to
         */
        // glm::mat4* getBaseWritePtr() { return linked() ? &resource->buffer[offset] : nullptr; }

        /**
         * @brief Marks the bones for transfer to the GPU
         *
         * @param skeleton The skeleton to transfer
         */
        void markForTransfer(com::Skeleton* skeleton);

        /**
         * @brief Returns whether or not the link is valid
         */
        bool linked() const { return resource != nullptr; }

        /**
         * @brief Returns the offset of the first bone in the global pool of bones
         */
        std::uint32_t getOffset() const { return offset; }

    private:
        SkeletalBonesResource* resource;
        std::uint32_t offset;
        std::uint32_t len;

        friend class SkeletalBonesResource;
    };

    /**
     * @brief Allocates space in the bones pool for the given entity's skeleton
     *
     * @param entity The entity being added to the scene
     * @param key The scene key of the entity
     * @return True if the bones were added, false otherwise
     */
    virtual bool allocateObject(ecs::Entity entity, scene::Key key) override;

    /**
     * @brief Removes the entity's bones from the pool
     *
     * @param entity The entity being removed
     * @param key The scene key of the entity
     */
    virtual void releaseObject(ecs::Entity entity, scene::Key key) override;

private:
    ecs::Registry* registry;
    util::RangeAllocatorUnbounded<std::uint32_t> allocator;
    std::uint32_t dirtyFrames;
    std::vector<com::Skeleton*> toTransfer;

    virtual void init(engine::Engine& engine, RenderTarget&) override;
    virtual bool dynamicDescriptorUpdateRequired() const override;
    virtual bool staticDescriptorUpdateRequired() const override;
    virtual void performTransfer() override;
};

} // namespace sri
} // namespace rc
} // namespace bl

#endif
