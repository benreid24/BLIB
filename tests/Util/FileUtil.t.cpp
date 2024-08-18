#include <BLIB/Util/FileUtil.hpp>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unordered_set>

namespace bl
{
namespace util
{
namespace unittest
{
namespace
{
void touch(const std::string& filename) {
    std::ofstream file(filename.c_str());
    file << "touched";
}
} // namespace

TEST(FileUtil, Extension) {
    const std::vector<std::pair<std::string, std::string>> files = {
        {"/some/path/file.png", "png"},
        {"some/other/path/file.info.json", "json"},
        {"some/werid.path/file.jpg", "jpg"},
        {"/woah/this.might/be/too.much.csv", "csv"}};
    for (unsigned int i = 0; i < files.size(); ++i) {
        EXPECT_EQ(FileUtil::getExtension(files[i].first), files[i].second);
    }
}

TEST(FileUtil, Filename) {
    const std::vector<std::pair<std::string, std::string>> files = {
        {"some/path/file.png", "file.png"},
        {"some/other/path/file.info.json", "file.info.json"},
        {"some/werid.path/file.jpg", "file.jpg"},
        {"/woah/this.might/be/too.much.csv", "too.much.csv"}};
    for (unsigned int i = 0; i < files.size(); ++i) {
        EXPECT_EQ(FileUtil::getFilename(files[i].first), files[i].second);
    }
}

TEST(FileUtil, Basename) {
    const std::vector<std::pair<std::string, std::string>> files = {
        {"some/path/file.png", "file"},
        {"some/other/path/file.info.json", "file.info"},
        {"some/werid.path/file.jpg", "file"},
        {"/woah/this.might/be/too.much.csv", "too.much"}};
    for (unsigned int i = 0; i < files.size(); ++i) {
        EXPECT_EQ(FileUtil::getBaseName(files[i].first), files[i].second);
    }
}

TEST(FileUtil, Path) {
    const std::vector<std::pair<std::string, std::string>> files = {
        {"some/path/file.png", "some/path"},
        {"some/other/path/file.info.json", "some/other/path"},
        {"some/weird.path/file.jpg", "some/weird.path"},
        {"/woah/this.might/be/too.much.csv", "/woah/this.might/be"}};
    for (unsigned int i = 0; i < files.size(); ++i) {
        EXPECT_EQ(FileUtil::getPath(files[i].first), files[i].second);
    }
}

TEST(FileUtil, JoinPath) {
    const std::vector<std::pair<std::string, std::string>> paths = {
        {"i/am/folder", "file.png"}, {"/oh/no/", "file.png"}, {"ohhh/noooo/", "/file.png"}};
    const std::vector<std::string> expected = {
        "i/am/folder/file.png", "/oh/no/file.png", "ohhh/noooo/file.png"};
    for (unsigned int i = 0; i < paths.size(); ++i) {
        EXPECT_EQ(FileUtil::joinPath(paths[i].first, paths[i].second), expected[i]);
    }
}

TEST(FileUtil, ReadFile) {
    const std::string content = "hello fish world";
    std::ofstream out("readfile.txt");
    out << content;
    out.close();

    std::string read;
    ASSERT_TRUE(FileUtil::readFile("readfile.txt", read));
    EXPECT_EQ(read, content);
}

TEST(FileUtil, ListDirectory) {
    ASSERT_TRUE(FileUtil::createDirectory("list_dir"));
    ASSERT_TRUE(FileUtil::createDirectory("list_dir/subdir"));

    std::unordered_set<std::string> expected(
        {"list_dir/file1.txt", "list_dir/file3.txt", "list_dir/subdir/file4.txt"});
    for (const std::string& file : expected) { touch(file); }
    touch("list_dir/file2.png");

    const std::vector<std::string> actual = FileUtil::listDirectory("list_dir", "txt", true);
    for (const std::string& file : actual) {
        const auto it = expected.find(file);
        EXPECT_NE(it, expected.end()) << file << " not expected";
        if (it != expected.end()) { expected.erase(it); }
    }

    EXPECT_TRUE(expected.empty());

    const std::vector<std::string> dirs = FileUtil::listDirectoryFolders("list_dir");
    ASSERT_EQ(dirs.size(), 1);
    EXPECT_EQ(dirs[0], "subdir");

    EXPECT_TRUE(FileUtil::deleteDirectory("list_dir"));
}

} // namespace unittest
} // namespace util
} // namespace bl
