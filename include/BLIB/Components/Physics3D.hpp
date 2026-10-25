#ifndef BLIB_COMPONENTS_PHYSICS3D_HPP
#define BLIB_COMPONENTS_PHYSICS3D_HPP

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <limits>

namespace bl
{
namespace com
{
class Physics3D {
public:
    enum struct ObjectLayer : JPH::ObjectLayer {
        Terrain = 0,
        Dynamic = 1,
        Bullet  = 2,
        Debris  = 3,
        Count   = 4
    };

    static constexpr std::size_t MaxObjectLayers = std::numeric_limits<JPH::ObjectLayer>::digits;

    enum struct BroadphaseLayer : JPH::BroadPhaseLayer::Type {
        Moving    = 0,
        NonMoving = 1,
        Count     = 2
    };

    static constexpr std::size_t MaxBroadphaseLayers =
        std::numeric_limits<JPH::BroadPhaseLayer::Type>::max();

    struct Bitmasks {
        static constexpr std::uint16_t TerrainBitmask = 0x1 << int(ObjectLayer::Terrain);
        static constexpr std::uint16_t DynamicBitmask = 0x1 << int(ObjectLayer::Dynamic);
        static constexpr std::uint16_t BulletBitmask  = 0x1 << int(ObjectLayer::Bullet);
        static constexpr std::uint16_t DebrisBitmask  = 0x1 << int(ObjectLayer::Debris);
        static constexpr std::uint16_t All            = 0xFFFF;
        static constexpr std::uint16_t None           = 0;

        static constexpr std::uint16_t Masks[] = {
            TerrainBitmask,
            DynamicBitmask,
            BulletBitmask,
            DebrisBitmask,
        };
    };

    struct ObjectCollisionMasks {
        static constexpr std::uint16_t TerrainMask = Bitmasks::None;
        static constexpr std::uint16_t DynamicMask =
            Bitmasks::TerrainBitmask | Bitmasks::DynamicBitmask;
        static constexpr std::uint16_t BulletMask =
            Bitmasks::TerrainBitmask | Bitmasks::DynamicBitmask;
        static constexpr std::uint16_t DebrisMask =
            Bitmasks::TerrainBitmask | Bitmasks::DynamicBitmask;

        static constexpr std::uint16_t Masks[] = {
            TerrainMask,
            DynamicMask,
            BulletMask,
            DebrisMask,
        };
    };

    struct BroadphaseObjectMapping {
        static constexpr BroadphaseLayer Mapping[] = {
            BroadphaseLayer::Moving,    // Terrain
            BroadphaseLayer::Moving,    // Dynamic
            BroadphaseLayer::Moving,    // Bullet
            BroadphaseLayer::NonMoving, // Debris
        };
    };

private:
    //
};

} // namespace com
} // namespace bl

#endif
