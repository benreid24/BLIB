#ifndef BLIB_RENDER_GRAPH_ASSETREF_HPP
#define BLIB_RENDER_GRAPH_ASSETREF_HPP

namespace bl
{
namespace rc
{
namespace rg
{
class Asset;

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
    AssetRef(Asset* asset);

    /**
     * @brief Assigns this handle to the given copy
     *
     * @param copy The handle to copy
     */
    AssetRef(const AssetRef& copy);

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
    ~AssetRef();

    /**
     * @brief Assigns this handle to the given copy
     *
     * @param copy The handle to copy
     * @return A reference to this handle
     */
    AssetRef& operator=(const AssetRef& copy);

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
    Asset& operator*() { return *asset; }

    /**
     * @brief Dereferences the handle
     */
    const Asset& operator*() const { return *asset; }

    /**
     * @brief Dereferences the handle
     */
    Asset* operator->() { return asset; }

    /**
     * @brief Dereferences the handle
     */
    const Asset* operator->() const { return asset; }

    /**
     * @brief Returns whether or not the handle is valid. Only valid handles should be dereferenced
     */
    bool valid() const { return asset != nullptr; }

    /**
     * @brief Dereferences the handle
     */
    Asset& get() { return *asset; }

    /**
     * @brief Dereferences the handle
     */
    const Asset& get() const { return *asset; }

private:
    Asset* asset;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
