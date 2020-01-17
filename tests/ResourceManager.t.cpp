#include <BLIB/Resources/ResourceManager.hpp>

#include <BLIB/Util/Random.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace unittest
{

struct MockLoader
{
    Resource<int>::Ref operator() (const std::string&) {
        return std::make_shared<int>(Random::get(0, 1000));
    }
};

TEST(ResourceManager, Timeout)
{
    ResourceManager<int, MockLoader> manager(1);
    const int first = *manager.load("uri");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<int>::Ref second = manager.load("uri");
    EXPECT_NE(first, *second);
}

TEST(ResourceManager, NoTimeout)
{
    ResourceManager<int, MockLoader> manager(1);
    const Resource<int>::Ref first = manager.load("uri");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const Resource<int>::Ref second = manager.load("uri");
    EXPECT_EQ(*first, *second);
}

}
}