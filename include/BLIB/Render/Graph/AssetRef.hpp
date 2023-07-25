#ifndef BLIB_RENDER_GRAPH_ASSETREF_HPP
#define BLIB_RENDER_GRAPH_ASSETREF_HPP

#include <BLIB/Render/Graph/Asset.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
/**
 * @brief Handle to an asset in an AssetPool
 *
 * @inroup Renderer
 */
class AssetRef {
public:
    /**
     * @brief Creates an empty handle
     */
    AssetRef()
    : asset(nullptr) {}

    /**
     * @brief Creates a handle to the given asset
     *
     * @param asset The asset to reference
     */
    AssetRef(Asset* asset)
    : asset(asset) {
        ++asset->refCount;
    }

    /**
     * @brief Assigns this handle to the given copy
     *
     * @param copy The handle to copy
     */
    AssetRef(const AssetRef& copy)
    : asset(copy.asset) {
        if (valid()) ++asset->refCount;
    }

    /**
     * @brief Assumes ownership from the given handle
     *
     * @param move The handle to copy from and invalidate
     */
    AssetRef(AssetRef&& move) noexcept
    : asset(move.asset) {
        move.asset = nullptr;
    }

    /**
     * @brief Decrements the ref count on the underlying asset if any
     */
    ~AssetRef() {
        if (valid()) --asset->refCount;
    }

    /**
     * @brief Assigns this handle to the given copy
     *
     * @param copy The handle to copy
     * @return A reference to this handle
     */
    AssetRef& operator=(const AssetRef& copy) {
        asset = copy.asset;
        if (valid()) ++asset->refCount;
        return *this;
    }

    /**
     * @brief Assumes ownership from the given handle
     *
     * @param move The handle to copy from and invalidate
     * @return A reference to this handle
     */
    AssetRef& operator=(AssetRef&& move) noexcept {
        asset      = move.asset;
        move.asset = nullptr;
        return *this;
    }

    /**
     * @brief Dereferences the handle
     */
    constexpr Asset& operator*() { return *asset; }

    /**
     * @brief Dereferences the handle
     */
    constexpr const Asset& operator*() const { return *asset; }

    /**
     * @brief Dereferences the handle
     */
    constexpr Asset* operator->() { return asset; }

    /**
     * @brief Dereferences the handle
     */
    constexpr const Asset* operator->() const { return asset; }

    /**
     * @brief Returns whether or not the handle is valid. Only valid handles should be dereferenced
     */
    constexpr bool valid() const { return asset != nullptr; }

    /**
     * @brief Dereferences the handle
     */
    constexpr Asset& get() { return *asset; }

    /**
     * @brief Dereferences the handle
     */
    constexpr const Asset& get() const { return *asset; }

private:
    Asset* asset;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
