#ifndef BLIB_RENDER_GRAPH_ASSETS_TAGGEDEMPTYASSET_HPP
#define BLIB_RENDER_GRAPH_ASSETS_TAGGEDEMPTYASSET_HPP

#include <BLIB/Render/Graph/Asset.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Empty asset that can be used for creating dependencies between tasks
 *
 * @ingroup Renderer
 */
class TaggedEmptyAsset : public rg::Asset {
public:
    /**
     * @brief Creates an empty asset with the given tag
     *
     * @param tag The tag of the asset
     * @param terminal Whether the asset is terminal
     */
    TaggedEmptyAsset(std::string_view tag, bool terminal)
    : Asset(tag, terminal) {}

    /**
     * @brief Destroys the asset
     */
    ~TaggedEmptyAsset() = default;

private:
    void doCreate(const rg::InitContext&) override {}
    void doPrepareForInput(const rg::ExecutionContext&) override {}
    void doStartOutput(const rg::ExecutionContext&) override {}
    void doEndOutput(const rg::ExecutionContext&) override {}
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
