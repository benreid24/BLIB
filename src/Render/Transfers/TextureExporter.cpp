#include <BLIB/Render/Transfers/TextureExporter.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace tfr
{
TextureExporter::TextureExporter(Renderer& owner)
: owner(owner) {
    queuedExports.reserve(4);
    inProgressExports.reserve(4);
}

void TextureExporter::cleanup() {
    queuedExports.clear();
    inProgressExports.clear();
    exports.clear();
}

void TextureExporter::release(TextureExport* te) {
    if (!te->imageReady()) {
        BL_LOG_WARN << "Releasing an in-progress texture export";
        te->wait();
    }

    std::unique_lock lock(mutex);
    for (auto it = exports.begin(); it != exports.end(); ++it) {
        if (&*it == te) {
            exports.erase(it);
            return;
        }
    }
}

void TextureExporter::onFrameEnd() {
    if (!inProgressExports.empty()) {
        std::unique_lock lock(mutex);
        for (auto it = inProgressExports.begin(); it != inProgressExports.end();) {
            if ((*it)->checkComplete()) { it = inProgressExports.erase(it); }
            else { ++it; }
        }
    }

    if (!queuedExports.empty()) {
        std::unique_lock lock(mutex);
        for (auto* te : queuedExports) {
            te->performCopy();
            inProgressExports.emplace_back(te);
        }
        queuedExports.clear();
    }
}

TextureExport* TextureExporter::exportSwapImage() {
    const auto& frame =
        owner.vulkanState().swapchain.swapFrameAtIndex(owner.vulkanState().currentFrameIndex());
    auto& te =
        exports.emplace_back(owner.vulkanState(),
                             *this,
                             frame.colorImage(),
                             VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                             VkExtent3D{frame.renderExtent().width, frame.renderExtent().height, 1},
                             VK_IMAGE_ASPECT_COLOR_BIT,
                             owner.vulkanState().swapchain.swapImageFormat());
    queuedExports.emplace_back(&te);
    return &te;
}

TextureExport* TextureExporter::exportTexture(const res::TextureRef& t) {
    auto& te = exports.emplace_back(owner.vulkanState(),
                                    *this,
                                    t->getCurrentImage(),
                                    t->getCurrentImageLayout(),
                                    VkExtent3D{t->rawSize().x, t->rawSize().y, 1},
                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                    t->getFormat());
    queuedExports.emplace_back(&te);
    return &te;
}

} // namespace tfr
} // namespace rc
} // namespace bl
