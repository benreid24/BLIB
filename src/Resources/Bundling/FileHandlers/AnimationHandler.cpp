#include <BLIB/Media/Graphics/AnimationData.hpp>
#include <BLIB/Resources/Bundling/FileHandlers/AnimationHandler.hpp>

#include <BLIB/Resources/Bundling/FileHandlers/DefaultHandler.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
bool AnimationHandler::processFile(const std::string& path, std::ostream& output,
                                   FileHandlerContext& ctx) {
    // TODO - determine spritesheet path and add it as a dependency
    DefaultHandler handler;
    return handler.processFile(path, output, ctx);
}

} // namespace bundle
} // namespace resource
} // namespace bl
