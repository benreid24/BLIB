#ifndef BLIB_FILES_UTIL_HPP
#define BLIB_FILES_UTIL_HPP

#include <string>
#include <vector>

namespace bl
{
/// Collection of classes to simplify file interactions
namespace file
{
/**
 * @brief Common utility functions for dealing with files and filenames
 *
 * @ingroup Files
 */
struct Util {
    /**
     * @brief Test whether the given file exists
     *
     * @param file The filename to test
     * @return bool True if the file can be read, false otherwise
     */
    static bool exists(const std::string& file);

    /**
     * @brief Test whether or not the given directory exists
     *
     * @param path Path to the directory to test
     * @return True if it exists, false otherwise
     */
    static bool directoryExists(const std::string& path);

    /**
     * @brief Returns the endianness of the current system. Endianness is abstracted
     *        away by File but may still be useful to know
     *
     * @return true System is Big Endian
     * @return false System is Little Endian
     */
    static bool isBigEndian();

    /**
     * @brief Returns the extention of the given filename.
     *        Example: "file.tmp.txt" extension is "txt"
     *
     * @param file The filename to parse
     * @return std::string The extension of the file, or empty if none
     */
    static std::string getExtension(const std::string& file);

    /**
     * @brief Returns the base name of the given file path
     *        Example: "folder/file.txt" base is "file"
     *
     * @param file The file name to parse
     * @return std::string The base name of the file
     */
    static std::string getBaseName(const std::string& file);

    /**
     * @brief Returns the filename portion of the given path
     *        Example: "folder/file.txt" filename is "file.txt"
     *
     * @param file The file path to parse
     * @return std::string The file name stripped of its path
     */
    static std::string getFilename(const std::string& file);

    /**
     * @brief Returns the directory path of the given filename
     *        Example: "folder/subfolder/file.txt" path is "folder/subfolder/"
     *
     * @param file The filename to parse
     * @return std::string The path, or empty if no path present
     */
    static std::string getPath(const std::string& file);

    /**
     * @brief Joins a file path with another path or filename. Takes care of slashes
     *        Example: "path/" + "/file.txt" becomes "path/file.txt"
     *
     * @param leftPath The first portion of the path
     * @param rightPath The second portion of the path or a filename
     * @return std::string The joined path
     */
    static std::string joinPath(const std::string& leftPath, const std::string& rightPath);

    /**
     * @brief Generates a temporary filename in the given path, guaranteed to not exist
     *
     * @param path The folder to ensure file uniqueness in
     * @param ext An optional extension to use
     * @return std::string A temporary filename that does not exist
     */
    static std::string genTempName(const std::string& path, const std::string& ext = "");

    /**
     * @brief Copies a file from src to dest on the file system
     *
     * @param src The file to copy
     * @param dest Where to copy it to
     */
    static void copyFile(const std::string& src, const std::string& dest);

    /**
     * @brief Creates a directory on the file system, creating subdirectories as necessary
     *
     * @param path Directory to create
     * @return bool True if the full directory path could be created or already exists
     */
    static bool createDirectory(const std::string& path);

    /**
     * @brief Lists files in the given directory and optionally filters by extension
     *
     * @param path The directory to search
     * @param ext Extension to filter by, empty for no filter
     * @param recursive Whether or not to include files in subdirectories
     * @return std::vector<std::string> List of file paths. Names include path parameter
     */
    static std::vector<std::string> listDirectory(const std::string& path,
                                                  const std::string& ext = "",
                                                  bool recursive         = true);

    /**
     * @brief Lists the folders in a given directory. Results are not joined with the root dir
     *
     * @param path The directory to search in
     * @return std::vector<std::string> Folders in the search directory
     */
    static std::vector<std::string> listDirectoryFolders(const std::string& path);

    /**
     * @brief Delets the file from the file system
     *
     * @param file Path of the file to delete
     * @return bool True if the file could be deleted, false otherwise
     */
    static bool deleteFile(const std::string& file);

    /**
     * @brief Recursively deletes the given directory and all contained files and subdirectories
     *
     * @param path The directory to remove
     * @return True if the directory was removed, false on error
     */
    static bool deleteDirectory(const std::string& path);
};

} // namespace file
} // namespace bl

#endif
