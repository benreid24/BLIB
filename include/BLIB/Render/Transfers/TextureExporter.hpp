#ifndef BLIB_RENDER_TRANSFERS_TEXTUREEXPORTER_HPP
#define BLIB_RENDER_TRANSFERS_TEXTUREEXPORTER_HPP

#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Transfers/TextureExport.hpp>
#include <list>
#include <mutex>

namespace bl
{
namespace rc
{
class Renderer;

namespace tfr
{
/**
 * @brief Export manager that handles copying device local images to host visible buffers
 *
 * @ingroup Transfers
 */
class TextureExporter {
public:
    /**
     * @brief Queues a transfer of the swap frame output for the end of the current frame
     *
     * @return An export object pointer that can be awaited
     */
    TextureExport* exportSwapImage();

    /**
     * @brief Queues a transfer of a texture for the end of the current frame
     *
     * @return An export object pointer that can be awaited
     */
    TextureExport* exportTexture(const res::TextureRef& texture);

private:
    Renderer& owner;
    std::mutex mutex;
    std::list<TextureExport> exports;
    std::vector<TextureExport*> queuedExports;
    std::vector<TextureExport*> inProgressExports;

    TextureExporter(Renderer& owner);
    void cleanup();
    void release(TextureExport* te);
    void onFrameEnd();

    friend class TextureExport;
    friend class ::bl::rc::Renderer;
};

} // namespace tfr
} // namespace rc
} // namespace bl

#endif
