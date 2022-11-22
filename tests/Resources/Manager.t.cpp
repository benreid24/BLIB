#include <BLIB/Resources.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace resource
{
namespace unittest
{
namespace
{
struct MockLoader : public Loader<int> {
    int val;

    MockLoader()
    : val(0) {}

    virtual Resource<int>::Ref load(const std::string&) override {
        return std::make_shared<int>(val++);
    }
};

struct MockLoader2 : public Loader<char> {
    char val;

    MockLoader2()
    : val('a') {}

    virtual Resource<char>::Ref load(const std::string&) override {
        return std::make_shared<char>(val++);
    }
};
} // namespace

TEST(ResourceManager, TimeoutAndForceCache) {
    MockLoader2 loader;
    Manager<char> manager(loader, 1);
    const char first = *manager.load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<char>::Ref second = manager.load("uri").data;
    EXPECT_NE(first, *second);

    // test force in cache
    const int value                          = *manager.load("uri").data;
    manager.loadMutable("uri")->forceInCache = true;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(value, *manager.load("uri").data);
}

TEST(ResourceManager, NoTimeout) {
    MockLoader loader;
    Manager<int> manager(loader, 1);
    const Resource<int>::Ref first = manager.load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<int>::Ref second = manager.load("uri").data;
    EXPECT_EQ(*first, *second);
}

} // namespace unittest
} // namespace resource
} // namespace bl
