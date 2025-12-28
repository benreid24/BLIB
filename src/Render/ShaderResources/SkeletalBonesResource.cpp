#include <BLIB/Render/ShaderResources/SkeletalBonesResource.hpp>

#include <BLIB/Components/Bone.hpp>
#include <BLIB/Components/Skeleton.hpp>
#include <BLIB/Components/SkeletonIndexLink.hpp>
#include <BLIB/Components/Transform3D.hpp>
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
    buffer.transferEveryFrame();
}

bool SkeletalBonesResource::allocateObject(ecs::Entity entity, scene::Key) {
    ecs::Transaction<ecs::tx::EntityRead,
                     ecs::tx::ComponentRead<com::Skeleton, com::SkeletonIndexLink>>
        tx(*registry);

    com::Skeleton* skeleton = registry->getComponent<com::Skeleton>(entity, tx);
    if (!skeleton) {
        ecs::Entity parent = registry->getEntityParent(entity, tx);
        skeleton           = registry->getComponent<com::Skeleton>(parent, tx);
        if (!skeleton || !skeleton->resourceLink.linked()) { return false; }
        com::SkeletonIndexLink* link = registry->getComponent<com::SkeletonIndexLink>(entity, tx);
        if (!link) { return false; }
        link->baseBoneIndex = skeleton->resourceLink.offset;
        link->markForUpdate();
        return true;
    }
    com::SkeletonIndexLink* link = registry->getComponent<com::SkeletonIndexLink>(entity, tx);
    if (!link) { return false; }
    tx.unlock();

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
    link->baseBoneIndex             = alloc.range.start;
    link->markForUpdate();

    return true;
}

void SkeletalBonesResource::releaseObject(ecs::Entity entity, scene::Key) {
    com::Skeleton* skeleton = registry->getComponent<com::Skeleton>(entity);
    if (!skeleton) { return; }

    allocator.release({skeleton->resourceLink.offset, skeleton->resourceLink.len});
}

void SkeletalBonesResource::performTransfer() {
    for (com::Skeleton* skeleton : toTransfer) {
        glm::mat4 globalInverse = glm::inverse(skeleton->worldTransform->getGlobalTransform());
        for (unsigned int i = 0; i < skeleton->bones.size(); ++i) {
            buffer[skeleton->resourceLink.offset + i] =
                globalInverse * skeleton->bones[i].transform->getGlobalTransform() *
                skeleton->bones[i].bone->boneOffset;
        }
        buffer.markDirty(skeleton->resourceLink.offset, skeleton->bones.size());
    }
    toTransfer.clear();

    dirtyFrames = dirtyFrames >> 1;
}

bool SkeletalBonesResource::dynamicDescriptorUpdateRequired() const { return dirtyFrames > 0; }

bool SkeletalBonesResource::staticDescriptorUpdateRequired() const { return dirtyFrames > 0; }

void SkeletalBonesResource::ComponentLink::markForTransfer(com::Skeleton* skeleton) {
    if (resource) { resource->toTransfer.emplace_back(skeleton); }
}

} // namespace sri
} // namespace rc
} // namespace bl
