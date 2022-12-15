#ifndef BLIB_RESOURCES_BUNDLING_FILEHANDLERCONTEXT_HPP
#define BLIB_RESOURCES_BUNDLING_FILEHANDLERCONTEXT_HPP

#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

namespace bl
{
namespace resource
{
namespace bundle
{
class BundleCreator;

/**
 * @brief Context data that is provided to FileHandler objects when processing files
 *
 * @ingroup Bundling
 *
 */
class FileHandlerContext {
public:
    /**
     * @brief Adds a file to be bundled with the current file being processed. Call this for
     *        dependency files (ie spritesheets, etc)
     *
     * @param path The file to add to the current bundle
     */
    void addDependencyFile(const std::string& path);

    /**
     * @brief Adds a file to the exclude list. Use this for files whose data is pulled into the
     *        bundle some other way and are not needed in the final bundle set
     *
     * @param path The file to exclude
     */
    void addExcludeFile(const std::string& path);

private:
    std::unordered_set<std::string>& excludes;
    std::stack<std::string, std::vector<std::string>>& fileList;

    FileHandlerContext(std::unordered_set<std::string>& excludes,
                       std::stack<std::string, std::vector<std::string>>& fileStack);

    friend class BundleCreator;
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif
