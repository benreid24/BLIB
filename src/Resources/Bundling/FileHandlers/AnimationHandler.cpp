#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Resources/Bundling/FileHandlers/AnimationHandler.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
bool AnimationHandler::processFile(const std::string& path, std::ostream& output,
                                   FileHandlerContext& ctx) {
    gfx::a2d::AnimationData anim;
    if (!anim.loadFromFileForBundling(path)) {
        BL_LOG_ERROR << "Failed to load animation: " << path;
        return false;
    }
    if (!anim.saveToBundle(output)) {
        BL_LOG_ERROR << "Failed to save animation: " << path;
        return false;
    }
    ctx.addDependencyFile(anim.spritesheetFile());
    return true;
}

} // namespace bundle
} // namespace resource
} // namespace bl
