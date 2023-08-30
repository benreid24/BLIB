#include <BLIB/Containers/IndexMappedList.hpp>
#include <BLIB/Util/Random.hpp>
#include <gtest/gtest.h>
#include <string_view>

namespace bl
{
namespace ctr
{
namespace unittest
{
TEST(IndexMappedList, SingleKeySingleValue) {
    IndexMappedList<std::uint32_t, int> map;

    EXPECT_FALSE(map.hasValues(5));
    map.add(5, 10);
    EXPECT_TRUE(map.hasValues(5));
    EXPECT_TRUE(map.hasValue(5, 10));
    for (int val : map.getValues(5)) { EXPECT_EQ(val, 10); }
    map.remove(5);
    EXPECT_FALSE(map.hasValues(5));
    EXPECT_FALSE(map.hasValue(5, 10));

    map.add(5, 10);
    EXPECT_TRUE(map.hasValues(5));
    EXPECT_TRUE(map.hasValue(5, 10));
    for (int val : map.getValues(5)) { EXPECT_EQ(val, 10); }
    map.removeValue(5, 10);
    EXPECT_FALSE(map.hasValues(5));
    EXPECT_FALSE(map.hasValue(5, 10));
}

TEST(IndexMappedList, SingleKeyMultiValue) {
    IndexMappedList<std::uint32_t, int> map(1);

    map.add(5, 5);
    map.add(5, 55);
    EXPECT_TRUE(map.hasValues(5));
    EXPECT_TRUE(map.hasValue(5, 5));
    EXPECT_TRUE(map.hasValue(5, 55));

    map.add(5, 555);
    EXPECT_TRUE(map.hasValue(5, 555));

    map.removeValue(5, 55);
    EXPECT_TRUE(map.hasValue(5, 5));
    EXPECT_FALSE(map.hasValue(5, 55));
    EXPECT_TRUE(map.hasValue(5, 555));

    map.remove(5);
    EXPECT_FALSE(map.hasValues(5));
}

TEST(IndexMappedList, RandomKeysAndValues) {
    constexpr std::size_t KeyCount = std::numeric_limits<std::uint16_t>::max() / 2;
    IndexMappedList<std::uint32_t, int> map;

    // generate random key -> value set
    std::vector<std::vector<int>> expected;
    expected.resize(KeyCount);
    for (auto& set : expected) {
        const std::uint32_t count = util::Random::get<std::uint32_t>(0, 50);
        set.reserve(count);
        for (std::uint32_t i = 0; i < count; ++i) {
            set.push_back(util::Random::get<int>(0, KeyCount));
        }
    }

    // add all values to set
    for (unsigned int key = 0; key < expected.size(); ++key) {
        for (int val : expected[key]) { map.add(key, val); }
    }

    std::vector<bool> removed(KeyCount, false);
    const auto verify = [&map, &expected, &removed](std::string_view ctx) {
        for (unsigned int key = 0; key < expected.size(); ++key) {
            if (expected[key].empty() || removed[key]) {
                ASSERT_FALSE(map.hasValues(key)) << ctx << " | Contains unexpected key: " << key;
            }
            else {
                ASSERT_TRUE(map.hasValues(key))
                    << ctx << " | Does not contain expected key: " << key;
                for (int val : expected[key]) {
                    ASSERT_TRUE(map.hasValue(key, val))
                        << ctx << " | key " << key << " Does not contain expected value: " << val;
                }
            }
        }
    };

    // verify initial condition
    verify("Initial Setup");

    // randomly delete some values and verify
    unsigned int rmCount = util::Random::get<unsigned int>(500, 5000);
    for (unsigned int i = 0; i < rmCount; ++i) {
        const std::uint32_t key = util::Random::get<std::uint32_t>(0, expected.size() - 1);
        if (!expected[key].empty()) {
            const unsigned int j = util::Random::get<unsigned int>(0, expected[key].size() - 1);
            map.removeValue(key, expected[key][j]);
            expected[key].erase(expected[key].begin() + j);
        }
    }
    verify("Random Value Deletion");

    // randomly delete some keys and verify
    rmCount = util::Random::get<unsigned int>(500, 5000);
    std::vector<unsigned int> toRm;
    for (unsigned int i = 0; i < rmCount; ++i) {
        const std::uint32_t key = util::Random::get<std::uint32_t>(0, expected.size() - 1);
        map.remove(key);
        removed[key] = true;
    }
    verify("Random Key Deletion");

    // clear and test all keys gone
    std::fill(removed.begin(), removed.end(), true);
    map.clear();
    verify("Clear");
}

} // namespace unittest
} // namespace ctr
} // namespace bl
