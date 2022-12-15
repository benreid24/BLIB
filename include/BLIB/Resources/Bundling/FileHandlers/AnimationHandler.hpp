#ifndef BLIB_RESOURCES_BUNDLING_FILEHANDLERS_ANIMATIONHANDLER_HPP
#define BLIB_RESOURCES_BUNDLING_FILEHANDLERS_ANIMATIONHANDLER_HPP

#include <BLIB/Resources/Bundling/FileHandler.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief File handler for animations. Copies the data from disk as-is and adds the spritesheet file
 *        to the bundle as well
 *
 * @ingroup Bundling
 *
 */
struct AnimationHandler : public FileHandler {
    virtual ~AnimationHandler() = default;
    virtual bool processFile(const std::string& path, std::ostream& output,
                             FileHandlerContext& context) override;
};

} // namespace bundle
} // namespace resource
} // namespace bl

#endif
