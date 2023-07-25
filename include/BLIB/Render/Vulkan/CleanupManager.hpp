#ifndef BLIB_RENDER_VULKAN_CLEANUPMANAGER_HPP
#define BLIB_RENDER_VULKAN_CLEANUPMANAGER_HPP

#include <BLIB/Render/Config.hpp>
#include <array>
#include <functional>
#include <vector>

namespace bl
{
namespace rc
{
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
     * @param cb The callback to invoke after Config::MaxConcurrentFrames frames have elapsed
     */
    void add(Callback&& cb);

    /**
     * @brief Adds the given callback to the queue
     *
     * @param cb The callback to invoke after Config::MaxConcurrentFrames frames have elapsed
     */
    void add(const Callback& cb);

private:
    std::array<std::vector<Callback>, Config::MaxConcurrentFrames + 1> buckets;
    unsigned int clearIndex;

    CleanupManager();
    void flush();
    void onFrameStart();

    constexpr unsigned int addIndex() const {
        return (clearIndex + static_cast<unsigned int>(Config::MaxConcurrentFrames)) %
               buckets.size();
    }

    friend struct VulkanState;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
