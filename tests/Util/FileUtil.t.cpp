#include <BLIB/Util/FileUtil.hpp>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(FileUtil, Extension) {
    const std::vector<std::pair<std::string, std::string>> files = {
        {"some/path/file.png", "png"},
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
        {"some/path/file.png", "some/path/"},
        {"some/other/path/file.info.json", "some/other/path/"},
        {"some/weird.path/file.jpg", "some/weird.path/"},
        {"/woah/this.might/be/too.much.csv", "/woah/this.might/be/"}};
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

} // namespace unittest
} // namespace util
} // namespace bl