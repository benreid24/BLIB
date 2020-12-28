#include <BENG/Files/BinaryFile.hpp>
#include <BENG/Files/FileUtil.hpp>
#include <gtest/gtest.h>

namespace bg
{

namespace unittest
{

TEST(BinaryFile, Integers) {
    ASSERT_TRUE(FileUtil::createDirectory("temp"));
    const std::string filename = FileUtil::genTempName("temp", "bin");

    //test write
    {
        BinaryFile file(filename, BinaryFile::Write);
        ASSERT_TRUE(file.good());
        ASSERT_TRUE(file.write<uint32_t>(100));
        ASSERT_TRUE(file.write<uint16_t>(1234));
        ASSERT_TRUE(file.write<uint8_t>(0));
        ASSERT_TRUE(file.write<int32_t>(-12345));
        ASSERT_TRUE(file.write<int16_t>(900));
        ASSERT_TRUE(file.write<int8_t>(-1));
        ASSERT_TRUE(file.write<bool>(true));
        ASSERT_TRUE(file.good());
    }

    //test read
    {
        BinaryFile file(filename, BinaryFile::Read);
        ASSERT_TRUE(file.good());

        uint32_t u32 = 0;
        ASSERT_TRUE(file.read<uint32_t>(u32));
        EXPECT_EQ(u32, 100);

        uint16_t u16 = 0;
        ASSERT_TRUE(file.read<uint16_t>(u16));
        EXPECT_EQ(u16, 1234);

        uint8_t u8 = 0;
        ASSERT_TRUE(file.read<uint8_t>(u8));
        EXPECT_EQ(u8, 0);

        int32_t s32 = 0;
        ASSERT_TRUE(file.read<int32_t>(s32));
        EXPECT_EQ(s32, -12345);

        int16_t s16 = 0;
        ASSERT_TRUE(file.read<int16_t>(s16));
        EXPECT_EQ(s16, 900);

        int8_t s8 = 0;
        ASSERT_TRUE(file.read<int8_t>(s8));
        EXPECT_EQ(s8, -1);

        bool b = false;
        ASSERT_TRUE(file.read<bool>(b));
        EXPECT_EQ(b, true);

        EXPECT_FALSE(file.good());
    }

    ASSERT_TRUE(FileUtil::deleteFile(filename));
}

TEST(BinaryFile, String) {
    ASSERT_TRUE(FileUtil::createDirectory("temp"));
    const std::string filename = FileUtil::genTempName("temp", "bin");

    //write
    {
        BinaryFile file(filename, BinaryFile::Write);
        ASSERT_TRUE(file.good());

        ASSERT_TRUE(file.write("Hello world"));
        ASSERT_TRUE(file.write("i am data!"));
    }

    //read
    {
        BinaryFile file(filename, BinaryFile::Read);
        ASSERT_TRUE(file.good());

        std::string value;
        ASSERT_TRUE(file.read(value));
        EXPECT_EQ("Hello world", value);
        ASSERT_TRUE(file.read(value));
        EXPECT_EQ("i am data!", value);
        EXPECT_FALSE(file.good());
    }

    ASSERT_TRUE(FileUtil::deleteFile(filename));
}

TEST(BinaryFile, Mixed) {
    ASSERT_TRUE(FileUtil::createDirectory("temp"));
    const std::string filename = FileUtil::genTempName("temp", "bin");

    //write
    {
        BinaryFile file(filename, BinaryFile::Write);
        ASSERT_TRUE(file.write<uint32_t>(123456));
        ASSERT_TRUE(file.write("data"));
        ASSERT_TRUE(file.write<int16_t>(-1234));
        ASSERT_TRUE(file.write("sandwich"));
        ASSERT_TRUE(file.good());
    }

    //read
    {
        BinaryFile file(filename, BinaryFile::Read);
        ASSERT_TRUE(file.good());

        uint32_t u32;
        ASSERT_TRUE(file.read<uint32_t>(u32));
        EXPECT_EQ(123456, u32);

        std::string str;
        ASSERT_TRUE(file.read(str));
        EXPECT_EQ("data", str);

        int16_t i16;
        ASSERT_TRUE(file.read<int16_t>(i16));
        EXPECT_EQ(-1234, i16);

        ASSERT_TRUE(file.read(str));
        EXPECT_EQ("sandwich", str);
        EXPECT_FALSE(file.good());
    }

    ASSERT_TRUE(FileUtil::deleteFile(filename));
}

}

}