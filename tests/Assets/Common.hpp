#ifndef TESTS_ASSETS_COMMON_HPP
#define TESTS_ASSETS_COMMON_HPP

#include <BLIB/Util/FileUtil.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace as
{
namespace unittest
{
constexpr const char* AssetDirectory  = "test_assets";
constexpr const char* BundleDirectory = "test_bundle";

class RepositoryTest : public ::testing::Test {
public:
    void SetUp() override {
        util::FileUtil::deleteDirectory(AssetDirectory);
        util::FileUtil::deleteDirectory(BundleDirectory);
    }
};
} // namespace unittest
} // namespace as
} // namespace bl

#endif
