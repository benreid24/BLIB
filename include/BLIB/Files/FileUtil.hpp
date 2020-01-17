#ifndef BLIB_FILES_FILEUTIL_HPP
#define BLIB_FILES_FILEUTIL_HPP

#include <string>
#include <vector>

namespace bl
{
/**
 * Common utility functions for dealing with files and filenames
 *
 * \ingroup Files
 */
struct FileUtil {
    static bool exists(const std::string& file);
    static bool isBigEndian();

    static std::string getExtension(const std::string& file);
    static std::string getBaseName(const std::string& file);
    static std::string getFilename(const std::string& file);
    static std::string getPath(const std::string& file);
    static std::string joinPath(const std::string& leftPath, const std::string& rightPath);

    static std::string genTempName(const std::string& path, const std::string& ext = "");

    static void copyFile(const std::string& src, const std::string& dest);
    static bool createDirectory(const std::string& path);
    static std::vector<std::string> listDirectory(
        const std::string& path, const std::string& ext = "", bool recursive = true);
    static bool deleteFile(const std::string& file);
};

} // namespace bl

#endif