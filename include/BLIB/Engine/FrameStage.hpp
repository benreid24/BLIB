#ifndef BLIB_ENGINE_FRAMESTAGE_HPP
#define BLIB_ENGINE_FRAMESTAGE_HPP

#include <cstdint>

namespace bl
{
namespace engine
{
/**
 * @brief Container for the names of each stage of a frame in the engine. Used by Systems to bucket
 *        systems by when they should run
 *
 * @ingroup Engine
 */
struct FrameStage {
    /// Value-type for engine stage ids
    enum V : std::uint32_t {
        /// Stages 0-2 are for user-defined systems. Note that State::update is called before any
        /// systems are called into
        FrameStart = 0,

        /// Generic update stage
        Update0 = 0,

        /// Generic update stage
        Update1 = 1,

        /// Generic update stage
        Update2 = 2,

        /// Renderer components such as Animation update here. Also when velocity is applied to
        /// transforms
        Animate = 3,

        /// This stage and earlier are called once per engine tick. Stages later than this are
        /// called once per frame (render + present). There may be more than one tick per frame
        MARKER_OncePerTick = 3,

        /// This stage and later are called once per frame
        MARKER_OncePerFrame = 4,

        /// Objects in scenes queued for removal are removed here
        RenderObjectRemoval = 5,

        /// Adds objects from the ECS into the scene-local object buffers for rendering
        RenderObjectInsertion = 6,

        /// Separate stage for descriptors to be modified prior to syncing into scene buffers
        RenderIntermediateRefresh = 7,

        /// Syncs dirty descriptor components (ie transforms) into scene-local descriptor sets
        RenderDescriptorRefresh = 8,

        /// Records command buffers and submits to the GPU
        Render = 9,

        /// Slot for more user-defined operations
        FrameEnd = 10,

        /// The number of engine stages
        COUNT = 11
    };
};

} // namespace engine
} // namespace bl

#endif
