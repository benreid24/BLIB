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
struct MockLoader : public LoaderBase<int> {
    int val;

    MockLoader()
    : val(0) {}

    virtual Resource<int>::Ref load(const std::string&, const char*, std::size_t,
                                    std::istream&) override {
        return std::make_shared<int>(val++);
    }
};

struct MockLoader2 : public LoaderBase<char> {
    char val;

    MockLoader2()
    : val('a') {}

    virtual Resource<char>::Ref load(const std::string&, const char*, std::size_t,
                                     std::istream&) override {
        return std::make_shared<char>(val++);
    }
};
} // namespace

TEST(ResourceManager, TimeoutAndForceCache) {
    ResourceManager<char>::installLoader<MockLoader2>();
    ResourceManager<char>::setGarbageCollectionPeriod(1);
    const char first = *ResourceManager<char>::load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<char>::Ref second = ResourceManager<char>::load("uri").data;
    EXPECT_NE(first, *second);

    // test force in cache
    const char value                                = *ResourceManager<char>::load("uri").data;
    ResourceManager<char>::load("uri").forceInCache = true;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(value, *ResourceManager<char>::load("uri").data);
}

TEST(ResourceManager, NoTimeout) {
    ResourceManager<int>::installLoader<MockLoader>();
    ResourceManager<int>::setGarbageCollectionPeriod(2);
    const Resource<int>::Ref first = ResourceManager<int>::load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    const Resource<int>::Ref second = ResourceManager<int>::load("uri").data;
    EXPECT_EQ(*first, *second);
}

} // namespace unittest
} // namespace resource
} // namespace bl
