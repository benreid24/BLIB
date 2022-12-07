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

    virtual bool load(const std::string&, const char*, std::size_t, std::istream&,
                      int& result) override {
        result = val++;
        return true;
    }
};

struct MockLoader2 : public LoaderBase<char> {
    char val;

    MockLoader2()
    : val('a') {}

    virtual bool load(const std::string&, const char*, std::size_t, std::istream&,
                      char& result) override {
        result = val++;
        return true;
    }
};

void createDummyFile() {
    std::ofstream out("uri");
    out << "data";
}

} // namespace

TEST(ResourceManager, TimeoutAndForceCache) {
    createDummyFile();

    ResourceManager<char>::installLoader<MockLoader2>();
    ResourceManager<char>::setGarbageCollectionPeriod(1);
    const char first = *ResourceManager<char>::load("uri").data;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    const Resource<char>::Ref second = ResourceManager<char>::load("uri").data;
    EXPECT_NE(first, *second);

    // test force in cache
    const char value                                = *ResourceManager<char>::load("uri").data;
    ResourceManager<char>::load("uri").forceInCache = true;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_EQ(value, *ResourceManager<char>::load("uri").data);
}

TEST(ResourceManager, NoTimeout) {
    createDummyFile();

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
