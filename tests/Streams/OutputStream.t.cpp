#include <BLIB/Streams/OutputStream.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace stream
{
namespace unittest
{
constexpr std::string_view TestFilePath = "test_files/test.txt";
constexpr std::string_view TestString   = "Hello, world!";

class OutputStreamTest : public ::testing::Test {
public:
    void SetUp() override { util::FileUtil::createDirectory("test_files"); }
    void TearDown() override { util::FileUtil::deleteDirectory("test_files"); }
};

TEST_F(OutputStreamTest, OpenFile) {
    OutputStream stream(TestFilePath);
    EXPECT_TRUE(stream.isValid());
    EXPECT_EQ(stream.getMode(), Mode::File);
    EXPECT_TRUE(stream.write(TestString.data(), TestString.size()));
    stream.close();

    std::string buf;
    ASSERT_TRUE(util::FileUtil::readFile(std::string(TestFilePath), buf));
    EXPECT_EQ(buf, TestString);
}

TEST_F(OutputStreamTest, OpenMemory) {
    OutputStream stream(TestString.size());
    EXPECT_TRUE(stream.isValid());
    EXPECT_EQ(stream.getMode(), Mode::Memory);
    EXPECT_TRUE(stream.write(TestString.data(), TestString.size()));
    const std::vector<char>* buf = stream.getBuffer();
    ASSERT_NE(buf, nullptr);
    EXPECT_EQ(std::string(buf->data(), buf->size()), TestString);
}

TEST_F(OutputStreamTest, OpenWrapper) {
    std::ostringstream oss;
    OutputStream stream(oss);
    EXPECT_TRUE(stream.isValid());
    EXPECT_EQ(stream.getMode(), Mode::Wrapper);
    EXPECT_TRUE(stream.write(TestString.data(), TestString.size()));
    EXPECT_EQ(oss.str(), TestString);
}

} // namespace unittest
} // namespace stream
} // namespace bl
