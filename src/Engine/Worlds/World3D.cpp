#include <BLIB/Engine/Worlds/World3D.hpp>

#include <BLIB/Systems/Jolt/Constants.hpp>
#include <BLIB/Systems/PathfindingSystem3D.hpp>
#include <BLIB/Systems/Physics3D.hpp>
#include <BLIB/Systems/SkeletalAnimationSystem.hpp>

namespace bl
{
namespace engine
{
namespace Layers
{
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING     = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}; // namespace Layers

namespace BroadPhaseLayers
{
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr std::uint32_t NUM_LAYERS(2);
}; // namespace BroadPhaseLayers

namespace
{
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
public:
    BPLayerInterfaceImpl() {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING]     = BroadPhaseLayers::MOVING;
    }

    virtual std::uint32_t GetNumBroadPhaseLayers() const override {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
        switch ((BroadPhaseLayer::Type)inLayer) {
        case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
            return "NON_MOVING";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
            return "MOVING";
        default:
            JPH_ASSERT(false);
            return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

} // namespace

World3D::World3D(Engine& owner)
: BasicWorld(owner) {
    auto& physics = owner.systems().registerSystemIfNotExists<sys::Physics3D>();
    owner.systems().registerSystemIfNotExists<sys::PathfindingSystem3D>();
    owner.systems().registerSystemIfNotExists<sys::SkeletalAnimationSystem>();

    // TODO - customize these
    BPLayerInterfaceImpl broadPhaseLayerInterface;
    JPH::ObjectVsBroadPhaseLayerFilter objectVsBroadPhaseLayerFilter;
    physicsSystem.Init(sys::jolt::Constants::MaxBodies,
                       sys::jolt::Constants::MaxBodyPairs,
                       sys::jolt::Constants::MaxContactConstraints,
                       sys::jolt::Constants::MaxBodyMutexes,
                       broadPhaseLayerInterface,
                       objectVsBroadPhaseLayerFilter,
                       physics.getObjectLayerPairFilter());
}

World3D::~World3D() {}

} // namespace engine
} // namespace bl
