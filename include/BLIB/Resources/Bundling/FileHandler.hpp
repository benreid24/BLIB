#ifndef BLIB_RESOURCES_BUNDLING_FILEHANDLER_HPP
#define BLIB_RESOURCES_BUNDLING_FILEHANDLER_HPP

#include <BLIB/Resources/Bundling/FileHandlerContext.hpp>
#include <ostream>
#include <string>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief Base class for file handlers. File handlers process raw files and pack them into bundles.
 *        The purpose of handlers is to register dependency files, exclude sidecar files, and
 *        perform conversions (ie json -> binary) before packing data into the bundle
 *
 * @ingroup Bundling
 */
struct FileHandler {
    /**
     * @brief Process the given file and add its data to the bundle
     *
     * @param path The file to process
     * @param output The stream to output packed data to
     * @param context Context to register dependency files and exclude files with
     * @return True if the file was able to be packed, false on error
     */
    virtual bool processFile(const std::string& path, std::ostream& output,
                             FileHandlerContext& context) = 0;

    /**
     * @brief Destroy the File Handler object
     *
     */
    virtual ~FileHandler() = default;
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif
