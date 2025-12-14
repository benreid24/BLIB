#include <BLIB/Render/ShaderResources/SkeletalBonesResource.hpp>

#include <BLIB/Components/Skeleton.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Config/Limits.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
SkeletalBonesResource::SkeletalBonesResource()
: allocator(512)
, dirtyFrames(0) {}

void SkeletalBonesResource::init(engine::Engine& engine, RenderTarget& owner) {
    BufferShaderResource::init(engine, owner);
    registry = &engine.ecs();
}

bool SkeletalBonesResource::allocateObject(ecs::Entity entity, scene::Key) {
    com::Skeleton* skeleton = registry->getComponent<com::Skeleton>(entity);
    if (!skeleton) { return false; }

    const auto alloc = allocator.alloc(skeleton->bones.size());
    if (alloc.poolExpanded) {
        buffer.resize(alloc.newPoolSize);
        dirtyFrames = 0x1 << cfg::Limits::MaxConcurrentFrames;
    }

    for (unsigned int i = 0; i < skeleton->bones.size(); ++i) {
        buffer[alloc.range.start + i] = glm::mat4(1.f);
    }

    skeleton->resourceLink.resource = this;
    skeleton->resourceLink.offset   = alloc.range.start;
    skeleton->resourceLink.len      = skeleton->bones.size();
    skeleton->needsRefresh          = true;
    skeleton->markForOffsetCopy();

    return true;
}

void SkeletalBonesResource::releaseObject(ecs::Entity entity, scene::Key) {
    com::Skeleton* skeleton = registry->getComponent<com::Skeleton>(entity);
    if (!skeleton) { return; }

    allocator.release({skeleton->resourceLink.offset, skeleton->resourceLink.len});
}

void SkeletalBonesResource::performTransfer() { dirtyFrames = dirtyFrames >> 1; }

bool SkeletalBonesResource::dynamicDescriptorUpdateRequired() const { return dirtyFrames > 0; }

bool SkeletalBonesResource::staticDescriptorUpdateRequired() const { return dirtyFrames > 0; }

void SkeletalBonesResource::ComponentLink::markForTransfer() {
    if (resource) { resource->buffer.markDirty(offset, len); }
}

} // namespace sri
} // namespace rc
} // namespace bl
