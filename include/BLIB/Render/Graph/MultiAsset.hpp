#ifndef BLIB_RENDER_GRAPH_MULTIASSET_HPP
#define BLIB_RENDER_GRAPH_MULTIASSET_HPP

#include <BLIB/Render/Graph/Asset.hpp>
#include <array>
#include <memory>
#include <utility>

namespace bl
{
namespace rc
{
namespace rg
{
/**
 * @brief Helper class that allows an asset to be duplicated N times while being treated as a single
 *        asset by the render graph
 *
 * @tparam T The type of asset to duplicate
 * @tparam N The number of duplicates to make
 */
template<typename T, std::uint32_t N>
class MultiAsset : public Asset {
public:
    /**
     * @brief Creates the assets
     *
     * @tparam TProvider The provider type to use to create the assets
     * @param tag The tag of this asset
     * @param provider The provider to use to create the assets
     */
    template<typename TProvider>
    MultiAsset(std::string_view tag, TProvider& provider);

    /**
     * @brief Destroys the assets
     */
    virtual ~MultiAsset() = default;

    /**
     * @brief Access the given asset
     *
     * @param i The index of the asset to get
     * @return The asset at the given index
     */
    T& operator[](std::uint32_t i);

    /**
     * @brief Access the given asset
     *
     * @param i The index of the asset to get
     * @return The asset at the given index
     */
    const T& operator[](std::uint32_t i) const;

    /**
     * @brief Access the given asset
     *
     * @param i The index of the asset to get
     * @return The asset at the given index
     */
    T& get(std::uint32_t i);

    /**
     * @brief Access the given asset
     *
     * @param i The index of the asset to get
     * @return The asset at the given index
     */
    const T& get(std::uint32_t i) const;

    /**
     * @brief Returns the number of assets contained
     */
    constexpr std::uint32_t size() const;

private:
    std::array<std::unique_ptr<T>, N> assets;

    virtual void doCreate(engine::Engine& engine, Renderer& renderer,
                          RenderTarget* observer) override;
    virtual void doPrepareForInput(const ExecutionContext& context) override;
    virtual void doPrepareForOutput(const ExecutionContext& context) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, std::uint32_t N>
template<typename TProvider>
MultiAsset<T, N>::MultiAsset(std::string_view tag, TProvider& provider)
: Asset(tag) {
    for (auto& a : assets) {
        Asset* newAsset = provider.create(tag);
        T* singleAsset  = dynamic_cast<T*>(newAsset);
        if (!singleAsset) {
            throw std::runtime_error("MultiAssetProvider must be used with MultiAsset");
        }
        a.reset(singleAsset);
    }
}

template<typename T, std::uint32_t N>
T& MultiAsset<T, N>::operator[](std::uint32_t i) {
    return *assets[i];
}

template<typename T, std::uint32_t N>
const T& MultiAsset<T, N>::operator[](std::uint32_t i) const {
    return *assets[i];
}

template<typename T, std::uint32_t N>
T& MultiAsset<T, N>::get(std::uint32_t i) {
    return *assets[i];
}

template<typename T, std::uint32_t N>
const T& MultiAsset<T, N>::get(std::uint32_t i) const {
    return *assets[i];
}

template<typename T, std::uint32_t N>
constexpr std::uint32_t MultiAsset<T, N>::size() const {
    return N;
}

template<typename T, std::uint32_t N>
void MultiAsset<T, N>::doCreate(engine::Engine& engine, Renderer& renderer,
                                RenderTarget* observer) {
    for (auto& asset : assets) { asset->create(engine, renderer, observer); }
}

template<typename T, std::uint32_t N>
void MultiAsset<T, N>::doPrepareForInput(const ExecutionContext& context) {
    for (auto& asset : assets) { asset->prepareForInput(context); }
}

template<typename T, std::uint32_t N>
void MultiAsset<T, N>::doPrepareForOutput(const ExecutionContext& context) {
    for (auto& asset : assets) { asset->prepareForOutput(context); }
}

} // namespace rg
} // namespace rc
} // namespace bl

#endif
