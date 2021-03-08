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

TEST(Manager, Timeout) {
    MockLoader loader;
    Manager<int> manager(loader, 1);
    const int first = *manager.load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<int>::Ref second = manager.load("uri").data;
    EXPECT_NE(first, *second);
}

TEST(Manager, NoTimeout) {
    MockLoader loader;
    Manager<int> manager(loader, 1);
    const Resource<int>::Ref first = manager.load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<int>::Ref second = manager.load("uri").data;
    EXPECT_EQ(*first, *second);
}

TEST(Manager, ForceInCache) {
    MockLoader loader;
    Manager<int> manager(loader, 1);
    const int value                          = *manager.load("uri").data;
    manager.loadMutable("uri")->forceInCache = true;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(value, *manager.load("uri").data);
}

} // namespace unittest
} // namespace resource
} // namespace bl
