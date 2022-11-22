#include <BLIB/Engine/Configuration.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace engine
{
namespace unittest
{
TEST(EngineConfiguration, SingleTypeManyKeys) {
    Configuration::clear();

    Configuration::set<int>("key1", 52);
    Configuration::set<int>("key2", -123);

    EXPECT_EQ(Configuration::get<int>("key1"), 52);
    EXPECT_EQ(Configuration::get<int>("key2"), -123);
}

TEST(EngineConfiguration, ManyTypesManyKeys) {
    Configuration::clear();

    Configuration::set<int>("key1", 124);
    Configuration::set<std::string>("key2", "str");

    EXPECT_EQ(Configuration::get<int>("key1"), 124);
    EXPECT_EQ(Configuration::get<std::string>("key2"), "str");
}

TEST(EngineConfiguration, SingleKeyManyTypes) {
    Configuration::clear();

    Configuration::set<int>("key", 5);
    Configuration::set<std::string>("key", "str");

    EXPECT_EQ(Configuration::get<int>("key"), 5);
    EXPECT_EQ(Configuration::get<std::string>("key"), "str");
}

TEST(EngineConfiguration, DefaultValues) {
    Configuration::clear();

    EXPECT_EQ(Configuration::getOrDefault<int>("key", 5), 5);
    EXPECT_EQ(Configuration::get<std::string>("key"), std::string());
}

TEST(EngineConfiguration, NoOverwrite) {
    Configuration::clear();

    Configuration::set<int>("key", 5, false);
    Configuration::set<int>("key", 10, false);

    EXPECT_EQ(Configuration::get<int>("key"), 5);
}

TEST(EngineConfiguration, Overwrite) {
    Configuration::clear();

    Configuration::set<int>("key", 5);
    Configuration::set<int>("key", 10, true);

    EXPECT_EQ(Configuration::get<int>("key"), 10);
}

TEST(EngineConfiguration, Clear) {
    Configuration::clear();

    Configuration::set<int>("key", 5);
    Configuration::clear();
    EXPECT_EQ(Configuration::getOrDefault<int>("key", 54), 54);
}

TEST(EngineConfiguration, SaveAndLoad) {
    Configuration::clear();

    Configuration::set<int>("int", -54);
    Configuration::set<unsigned int>("uint", 100);
    Configuration::set<float>("float", -563.5);
    Configuration::set<bool>("BLIB.cool", true);
    Configuration::set<std::string>("str", "some value");

    ASSERT_TRUE(Configuration::save("config.cfg"));
    Configuration::clear();
    ASSERT_TRUE(Configuration::load("config.cfg"));

    EXPECT_EQ(Configuration::get<int>("int"), -54);
    EXPECT_EQ(Configuration::get<unsigned int>("uint"), 100);
    EXPECT_EQ(Configuration::get<float>("float"), -563.5);
    EXPECT_EQ(Configuration::get<bool>("BLIB.cool"), true);
    EXPECT_EQ(Configuration::get<std::string>("str"), "some value");
}

} // namespace unittest
} // namespace engine
} // namespace bl
