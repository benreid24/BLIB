#ifndef BLIB_ASSETS_ASSET_HPP
#define BLIB_ASSETS_ASSET_HPP

#include <BLIB/Assets/Context.hpp>
#include <BLIB/Assets/Metadata.hpp>
#include <BLIB/Assets/RepoDependency.hpp>
#include <BLIB/Assets/State.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Util/UUID.hpp>
#include <atomic>
#include <memory>
#include <vector>

namespace bl
{
/// Contains the Asset system
namespace as
{
namespace detail
{
class DriverBase;
}
class Payload;
class Repository;
class Ref;

/**
 * @brief Represents an asset in the repository. Contains the metadata and possibly a payload
 *
 * @ingroup Assets
 */
class Asset {
public:
    /**
     * @brief Creates an empty asset. Does not leave the asset in a valid state unless called by the
     *        repository
     */
    Asset();

    /**
     * @brief Creates an empty asset
     *
     * @param repo The repository this asset belongs to
     */
    Asset(Repository& repo);

    /**
     * @brief Returns the UUID of the asset
     */
    const util::UUID& getUUID() const { return uuid; }

    /**
     * @brief Returns the type of the asset
     */
    const std::string& getType() const { return type; }

    /**
     * @brief Returns the metadata of the asset
     */
    Metadata& getMetadata() { return metadata; }

    /**
     * @brief Returns the metadata of the asset
     */
    const Metadata& getMetadata() const { return metadata; }

    /**
     * @brief Returns the state of the asset
     */
    State getState() const { return state.load(); }

    /**
     * @brief Returns the payload of the asset. Must only be called if the state is Loaded
     */
    Payload& getPayload();

    /**
     * @brief Returns the payload of the asset. Must only be called if the state is Loaded
     */
    const Payload& getPayload() const;

    /**
     * @brief Loads the payload. Will generally be called for you, but may need to be called
     *        manually if the asset is explicitly unloaded. Maintaining a Ref to an asset will
     *        prevent automatic unloading
     *
     * @return True if the payload was able to be loaded, false otherwise
     */
    bool load();

    /**
     * @brief Frees the payload if no Refs remain or if forced
     *
     * @param force Always release the payload regardless of ref count
     * @return True if the payload is not loaded, false otherwise
     */
    bool unload(bool force = false);

    /**
     * @brief Reloads the asset from its source file if it has one
     */
    bool reloadFromSource();

private:
    Repository* repo;
    util::UUID uuid;
    std::string type;
    Metadata metadata;
    std::unique_ptr<Payload> payload;
    mutable std::atomic<State> state;
    std::vector<RepoDependency> dependencies;
    std::atomic<unsigned int> refCount;

    bool initAfterDeserialize(Repository& repo);
    bool create(const CreateContext::CreateData& data);
    bool saveEditor();
    bool writePayload();
    detail::DriverBase* getDriver() const;

    // Called by Payload
    void flushPayload();

    // Called by metadata
    void handleDisplayNameChange(const std::string& oldName, const std::string& newName);
    void handlePathChange(const std::string& oldPath, const std::string& newPath);
    void handleAutoLoadChange();

    // Called by Repository
    void markReadyForAutoSync();

    friend class Repository;
    friend class Ref;
    friend struct refl::ReflectedObject<as::Asset>;
    friend class Payload;
    friend class Metadata;
};

} // namespace as

namespace refl
{
template<>
struct ReflectedObject<as::Asset> {
    inline static const auto spec = makeSpec<as::Asset>(
        "Asset", memberList(defineMember(1, "uuid", &as::Asset::uuid),
                            defineMember(2, "type", &as::Asset::type),
                            defineMember(3, "metadata", &as::Asset::metadata),
                            defineMember(4, "dependencies", &as::Asset::dependencies)));
};
} // namespace refl

} // namespace bl

#endif
