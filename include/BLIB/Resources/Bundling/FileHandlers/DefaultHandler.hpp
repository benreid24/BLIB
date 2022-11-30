#ifndef BLIB_RESOURCES_BUNDLING_FILEHANDLERS_DEFAULTHANDLER_HPP
#define BLIB_RESOURCES_BUNDLING_FILEHANDLERS_DEFAULTHANDLER_HPP

#include <BLIB/Resources/Bundling/FileHandler.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief Default file handler used for all files that do not have a matching handler. Simply copies
 *        the file as-is into the bundle
 *
 * @ingroup Bundling
 *
 */
struct DefaultHandler : public FileHandler {
    virtual bool processFile(const std::string& path, std::ostream& output,
                             FileHandlerContext& context) override;
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif
