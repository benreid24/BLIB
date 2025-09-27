#ifndef BLIB_RENDER_GRAPH_PROVIDERS_SIMPLEASSETPROVIDER_HPP
#define BLIB_RENDER_GRAPH_PROVIDERS_SIMPLEASSETPROVIDER_HPP

#include <BLIB/Render/Graph/AssetProvider.hpp>
#include <BLIB/Render/Graph/Assets/StandardTargetAsset.hpp>
#include <BLIB/Vulkan.hpp>
#include <type_traits>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Provider for StandardTargetAsset assets or other simple assets that only take the tag as
 *        the constructor argument or no arguments
 *
 * @tparam TAsset The asset type to create
 * @ingroup Renderer
 */
template<typename TAsset>
class SimpleAssetProvider : public rg::AssetProvider {
public:
    /**
     * @brief Creates the provider
     *
     * @param terminal Whether the created assets should be terminal
     */
    SimpleAssetProvider(bool terminal)
    : terminal(terminal) {}

private:
    bool terminal;

    virtual rg::Asset* create(std::string_view tag) override {
        if constexpr (std::is_constructible_v<TAsset, std::string_view>) { return new TAsset(tag); }
        else if constexpr (std::is_constructible_v<TAsset, std::string_view, bool>) {
            return new TAsset(tag, terminal);
        }
        else { return new TAsset(); }
    }
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
