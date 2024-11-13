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

        /// Stage where physics are applied and resolved
        Physics = 2,

        /// Generic update stage
        Update2 = 3,

        /// Generic update stage
        Update3 = 4,

        /// Renderer components such as Animation update here. Also when velocity is applied to
        /// transforms
        Animate = 5,

        /// This stage and earlier are called once per engine tick. Stages later than this are
        /// called once per frame (render + present). There may be more than one tick per frame
        MARKER_OncePerTick = 5,

        /// This stage and later are called once per frame
        MARKER_OncePerFrame = 6,

        /// Separate stage for descriptors to be modified prior to syncing into scene buffers
        RenderEarlyRefresh = 6,

        /// Syncs dirty descriptor components (ie transforms) into scene-local descriptor sets
        RenderDescriptorRefresh = 7,

        /// The number of engine stages
        COUNT = 8
    };
};

} // namespace engine
} // namespace bl

#endif
