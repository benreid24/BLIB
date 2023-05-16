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

        /// Renderer components such as Animation update here
        Animate = 3,

        /// This stage and earlier are called once per engine tick. Stages later than this are
        /// called once per frame (render + present). There may be more than one tick per frame
        MARKER_OncePerTick = 3,

        /// This stage and later are called once per frame
        MARKER_OncePerFrame = 4,

        /// Synchronizes objects from the ECS into the scene-local object buffers
        RenderObjectSync = 5,

        /// Syncs dirty descriptor components (ie transforms) into scene-local descriptor sets
        RenderDescriptorRefresh = 6,

        /// Records command buffers and submits to the GPU
        Render = 7,

        /// Slot for more user-defined operations
        FrameEnd = 8,

        /// The number of engine stages
        COUNT = 9
    };
};

} // namespace engine
} // namespace bl

#endif
