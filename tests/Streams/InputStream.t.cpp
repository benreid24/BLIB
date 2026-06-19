#include <BLIB/Streams/InputStream.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <gtest/gtest.h>
#include <sstream>

namespace bl
{
namespace stream
{
namespace unittest
{
constexpr std::string_view TestFilePath = "test_files/test.txt";
constexpr std::string_view TestString   = "Hello, world!";

class InputStreamTest : public ::testing::Test {
public:
    void SetUp() override {
        util::FileUtil::createDirectory("test_files");
        std::ofstream("test_files/test.txt") << TestString;
    }

    void TearDown() override { util::FileUtil::deleteDirectory("test_files"); }
};

TEST_F(InputStreamTest, OpenFile) {
    InputStream stream(TestFilePath);
    EXPECT_TRUE(stream.isValid());
    EXPECT_EQ(stream.getMode(), Mode::File);

    std::vector<char> buf;
    EXPECT_EQ(stream.read(buf, TestString.size()), TestString.size());
    EXPECT_EQ(std::string(buf.data(), buf.size()), TestString);
}

TEST_F(InputStreamTest, OpenMemory) {
    std::vector<char> data(TestString.begin(), TestString.end());
    InputStream stream(data);
    EXPECT_TRUE(stream.isValid());
    EXPECT_EQ(stream.getMode(), Mode::Memory);
    std::vector<char> buf;
    EXPECT_EQ(stream.read(buf, TestString.size()), TestString.size());
    EXPECT_EQ(std::string(buf.data(), buf.size()), TestString);
}

TEST_F(InputStreamTest, OpenWrapper) {
    std::stringstream iss;
    iss << TestString;
    InputStream stream(iss, TestString.size());
    EXPECT_TRUE(stream.isValid());
    EXPECT_EQ(stream.getMode(), Mode::Wrapper);
    std::vector<char> buf;
    EXPECT_EQ(stream.read(buf, TestString.size()), TestString.size());
    EXPECT_EQ(std::string(buf.data(), buf.size()), TestString);
}

} // namespace unittest
} // namespace stream
} // namespace bl
