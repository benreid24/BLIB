#include <BLIB/Resources.hpp>

#include <BLIB/Util/Random.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace resource
{
namespace unittest
{
struct MockLoader : public Loader<int> {
    virtual Resource<int>::Ref load(const std::string&) override {
        return std::make_shared<int>(util::Random::get(0, 1000));
    }
};

TEST(ResourceManager, Timeout) {
    MockLoader loader;
    Manager<int> manager(loader, 1);
    GarbageCollector collector;
    BL_LOG_INFO << "all running";
    const int first = *manager.load("uri").data;
    BL_LOG_INFO << "loaded data: " << first;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    BL_LOG_INFO << "slept";
    const Resource<int>::Ref second = manager.load("uri").data;
    BL_LOG_INFO << "got second: " << *second;
    EXPECT_NE(first, *second);
}

TEST(ResourceManager, NoTimeout) {
    MockLoader loader;
    Manager<int> manager(loader, 1);
    const Resource<int>::Ref first = manager.load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<int>::Ref second = manager.load("uri").data;
    EXPECT_EQ(*first, *second);
}

TEST(ResourceManager, ForceInCache) {
    MockLoader loader;
    Manager<int> manager(loader, 1);
    GarbageCollector collector;
    const int value                          = *manager.load("uri").data;
    manager.loadMutable("uri")->forceInCache = true;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(value, *manager.load("uri").data);
}

} // namespace unittest
} // namespace resource
} // namespace bl
