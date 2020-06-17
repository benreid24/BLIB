#include <BLIB/Resources/ResourceManager.hpp>

#include <BLIB/Util/Random.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{
struct MockLoader : public ResourceLoader<int> {
    virtual Resource<int>::Ref load(const std::string&) override {
        return std::make_shared<int>(Random::get(0, 1000));
    }
};

TEST(ResourceManager, Timeout) {
    MockLoader loader;
    ResourceManager<int> manager(loader, 1);
    const int first = *manager.load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<int>::Ref second = manager.load("uri").data;
    EXPECT_NE(first, *second);
}

TEST(ResourceManager, NoTimeout) {
    MockLoader loader;
    ResourceManager<int> manager(loader, 1);
    const Resource<int>::Ref first = manager.load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<int>::Ref second = manager.load("uri").data;
    EXPECT_EQ(*first, *second);
}

TEST(ResourceManager, ForceInCache) {
    MockLoader loader;
    ResourceManager<int> manager(loader, 1);
    const int value                          = *manager.load("uri").data;
    manager.loadMutable("uri")->forceInCache = true;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(value, *manager.load("uri").data);
}

} // namespace unittest
} // namespace bl