#include <BLIB/Render/Vulkan/CleanupManager.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
CleanupManager::CleanupManager()
: clearIndex(0) {
    for (auto& v : buckets) { v.reserve(64); }
}

void CleanupManager::flush() {
    for (auto& bucket : buckets) {
        for (auto& cb : bucket) { cb(); }
    }
}

void CleanupManager::add(const Callback& cb) { buckets[addIndex()].emplace_back(cb); }

void CleanupManager::add(Callback&& cb) {
    buckets[addIndex()].emplace_back(std::forward<Callback>(cb));
}

void CleanupManager::onFrameStart() {
    for (auto& cb : buckets[clearIndex]) { cb(); }
    buckets[clearIndex].clear();
    clearIndex = clearIndex < Config::MaxConcurrentFrames ? clearIndex + 1 : 0;
}

} // namespace vk
} // namespace rc
} // namespace bl
