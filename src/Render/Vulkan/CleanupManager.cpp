#include <BLIB/Render/Vulkan/CleanupManager.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
CleanupManager::CleanupManager()
: clearIndex(0)
, isClearing(false) {
    for (auto& v : buckets) { v.reserve(64); }
    addedDuringClear.reserve(4);
}

void CleanupManager::flush() {
    std::unique_lock lock(mutex);
    for (auto& bucket : buckets) {
        for (auto& cb : bucket) { cb(); }
    }
}

void CleanupManager::add(const Callback& cb) {
    if (!isClearing) {
        std::unique_lock lock(mutex);
        buckets[addIndex()].emplace_back(cb);
    }
    else { addedDuringClear.emplace_back(cb); }
}

void CleanupManager::add(Callback&& cb) {
    if (!isClearing) {
        std::unique_lock lock(mutex);
        buckets[addIndex()].emplace_back(std::forward<Callback>(cb));
    }
    else { addedDuringClear.emplace_back(std::forward<Callback>(cb)); }
}

void CleanupManager::onFrameStart() {
    isClearing = true;
    std::unique_lock lock(mutex);

    for (auto& cb : buckets[clearIndex]) { cb(); }
    const auto indexToClear = clearIndex;
    clearIndex              = clearIndex < cfg::Limits::MaxConcurrentFrames ? clearIndex + 1 : 0;

    lock.unlock();
    isClearing = false;

    buckets[indexToClear].swap(addedDuringClear);
    addedDuringClear.clear();
}

} // namespace vk
} // namespace rc
} // namespace bl
