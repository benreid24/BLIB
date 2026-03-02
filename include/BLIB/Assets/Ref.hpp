#ifndef BLIB_ASSETS_REF_HPP
#define BLIB_ASSETS_REF_HPP

#include <BLIB/Assets/State.hpp>

namespace bl
{
namespace as
{
class Repository;
class Asset;

/**
 * @brief Handle to an asset in the repository. Performs ref counting and prevents automatic
 *        unloading of assets while refs point to them. Assets can be manually unloaded if needed
 *
 * @ingroup Assets
 */
class Ref {
public:
    /**
     * @brief Creates an empty ref
     */
    Ref();

    /**
     * @brief Copies the ref, increasing the reference count of the asset
     *
     * @param copy The ref to copy from
     */
    Ref(const Ref& copy);

    /**
     * @brief Adopts the given ref
     *
     * @param move The ref to move from
     */
    Ref(Ref&& move);

    /**
     * @brief Releases the ref, decreasing the reference count
     */
    ~Ref();

    /**
     * @brief Copies the ref, increasing the reference count of the asset
     *
     * @param copy The ref to copy from
     * @return A reference to this object
     */
    Ref& operator=(const Ref& copy);

    /**
     * @brief Adopts the given ref
     *
     * @param move The ref to move from
     * @return A reference to this object
     */
    Ref& operator=(Ref&& move);

    /**
     * @brief Returns whether this ref is valid and points to an asset
     */
    bool isValid() const;

    /**
     * @brief Returns whether this ref is valid and points to an asset
     */
    operator bool() const;

    /**
     * @brief Returns the state of the underlying asset, or State::Unknown if this ref is not valid
     */
    State getState() const;

    /**
     * @brief Returns the underlying asset. Must only be called if the ref is valid
     */
    Asset& getAsset();

    /**
     * @brief Returns the underlying asset. Must only be called if the ref is valid
     */
    const Asset& getAsset() const;

    /**
     * @brief Returns a pointer to the underlying asset, must only be called if the ref is valid
     */
    Asset* operator->();

    /**
     * @brief Returns a pointer to the underlying asset, must only be called if the ref is valid
     */
    const Asset* operator->() const;

    /**
     * @brief Manually releases the ref, decrementing the reference count
     */
    void release();

    /**
     * @brief Returns the reference count of the underlying asset, or 0 if this ref is not valid
     */
    unsigned int getCount() const;

private:
    Repository* repo;
    Asset* asset;

    Ref(Repository* repo, Asset* asset);
    void addRef();
    void removeRef();

    friend class Repository;
};

} // namespace as
} // namespace bl

#endif
