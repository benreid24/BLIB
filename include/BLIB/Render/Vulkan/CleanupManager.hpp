#ifndef BLIB_RENDER_VULKAN_CLEANUPMANAGER_HPP
#define BLIB_RENDER_VULKAN_CLEANUPMANAGER_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <array>
#include <atomic>
#include <functional>
#include <mutex>
#include <vector>

namespace bl
{
namespace rc
{
class Renderer;

namespace vk
{
/**
 * @brief Utility to defer resource destruction for several frames to ensure resources are no longer
 *        in use without explicit synchronization or stalling
 *
 * @ingroup Renderer
 */
class CleanupManager {
public:
    /// Signature of cleanup callbacks
    using Callback = std::function<void()>;

    /**
     * @brief Adds the given callback to the queue
     *
     * @param cb The callback to invoke after cfg::Limits::MaxConcurrentFrames frames have elapsed
     */
    void add(Callback&& cb);

    /**
     * @brief Adds the given callback to the queue
     *
     * @param cb The callback to invoke after cfg::Limits::MaxConcurrentFrames frames have elapsed
     */
    void add(const Callback& cb);

private:
    std::mutex mutex;
    std::atomic_bool isClearing;
    std::array<std::vector<Callback>, cfg::Limits::MaxConcurrentFrames + 1> buckets;
    std::vector<Callback> addedDuringClear;
    unsigned int clearIndex;

    CleanupManager();
    void flush();
    void onFrameStart();

    constexpr unsigned int addIndex() const {
        return (clearIndex + static_cast<unsigned int>(cfg::Limits::MaxConcurrentFrames)) %
               buckets.size();
    }

    friend struct VulkanLayer;
    friend class Renderer;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
