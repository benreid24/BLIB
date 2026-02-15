#ifndef BLIB_ASSETS_ASSET_HPP
#define BLIB_ASSETS_ASSET_HPP

#include <BLIB/Assets/Metadata.hpp>
#include <BLIB/Assets/State.hpp>
#include <BLIB/Util/UUID.hpp>

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
    State getState() const { return state; }

    /**
     * @brief Returns the payload of the asset. Must only be called if the state is Loaded
     */
    Payload& getPayload();

private:
    util::UUID uuid;
    std::string type;
    Metadata metadata;
    Payload* payload;
    State state;
};

} // namespace as
} // namespace bl

#endif
