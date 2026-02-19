#ifndef BLIB_ASSETS_ASSET_HPP
#define BLIB_ASSETS_ASSET_HPP

#include <BLIB/Assets/Metadata.hpp>
#include <BLIB/Assets/State.hpp>
#include <BLIB/Util/UUID.hpp>
#include <atomic>

namespace bl
{
/// Contains the Asset system
namespace as
{
class Payload;

/**
 * @brief Represents an asset in the repository. Contains the metadata and possibly a payload
 *
 * @ingroup Assets
 */
class Asset {
public:
    /**
     * @brief Creates an empty asset
     */
    Asset();

    /**
     * @brief Returns the UUID of the asset
     */
    const util::UUID& getUUID() const { return uuid; }

    /**
     * @brief Returns the type of the asset
     */
    const std::string_view& getType() const { return type; }

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
     * @brief Loads the payload. Will generally be called for you, but may need to be called
     *        manually if the asset is explicitly unloaded. Maintaining a Ref to an asset will
     *        prevent automatic unloading
     *
     * @return True if the payload was able to be loaded, false otherwise
     */
    bool load();

private:
    util::UUID uuid;
    std::string_view type;
    Metadata metadata;
    Payload* payload;
    std::atomic<State> state;
};

} // namespace as
} // namespace bl

#endif
