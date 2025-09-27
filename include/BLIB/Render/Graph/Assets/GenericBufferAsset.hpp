#ifndef BLIB_RENDER_GRAPH_ASSETS_GENERICBUFFERASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_GENERICBUFFERASSET_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Intermediate asset type meant to be derived from assets intended to be used with
 *        the GenericBufferBinding descriptor set helper
 *
 * @ingroup Renderer
 */
class GenericBufferAsset : public rg::Asset {
public:
    /**
     * @brief Creates the work buffer asset
     *
     * @param tag The tag of the asset
     * @param terminal Indicates if this is a terminal asset
     */
    GenericBufferAsset(std::string_view tag, bool terminal)
    : Asset(tag, terminal) {}

    /**
     * @brief Destroys the asset
     */
    virtual ~GenericBufferAsset() = default;

    /**
     * @brief Returns the underlying Vulkan buffer
     */
    vk::Buffer& getBuffer() { return buffer; }

protected:
    vk::Buffer buffer;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
