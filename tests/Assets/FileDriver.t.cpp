#include <BLIB/Assets/Builtin/FilePayload.hpp>
#include <BLIB/Assets/Drivers/FileDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

#include <fstream>
#include <string>

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, FileDriver) {
    const std::string sourceFilePath = "test_file_payload_source.bin";
    const std::string sourceContent  = "Hello, FilePayload! Binary: \x01\x02\x03\xFF";

    // Write the source file that the driver will read from
    {
        std::ofstream out(sourceFilePath, std::ios::binary);
        ASSERT_TRUE(out.is_open());
        out.write(sourceContent.data(), static_cast<std::streamsize>(sourceContent.size()));
    }

    util::UUID uuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        as::CreateContext::CreateData createData(sourceFilePath);
        auto asset = repo.createAsset<asi::FilePayload>("TestFile", createData);
        ASSERT_TRUE(asset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        uuid = asset.getUUID();
    }

    util::FileUtil::deleteFile(sourceFilePath);

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<asi::FilePayload>(uuid);
    ASSERT_TRUE(asset.isValid());

    const std::vector<char>& data = asset->getData();
    ASSERT_EQ(data.size(), sourceContent.size());
    EXPECT_EQ(std::string(data.begin(), data.end()), sourceContent);
}

} // namespace unittest
} // namespace as
} // namespace bl
