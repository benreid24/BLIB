#ifndef BLIB_ASSETS_METADATA_HPP
#define BLIB_ASSETS_METADATA_HPP

#include <chrono>
#include <cstdint>
#include <string>

namespace bl
{
namespace as
{
/**
 * @brief Asset metadata storage
 *
 * @ingroup Assets
 */
class Metadata {
public:
    /**
     * @brief Creates default metadata
     */
    Metadata();

    /**
     * @brief Copies the metadata
     *
     * @param copy The metadata to copy
     */
    Metadata(const Metadata& copy);

    /**
     * @brief Returns the display name of the asset
     */
    const std::string& getDisplayName() const { return displayName; }

    /**
     * @brief Returns the description of the asset
     */
    const std::string& getDescription() const { return description; }

    /**
     * @brief Returns the creation time of the asset as a raw timestamp
     */
    std::uint64_t getCreationTimeRaw() const { return creationTime; }

    /**
     * @brief Returns the creation time of the asset as a sys_time
     */
    std::chrono::sys_time<std::chrono::seconds> getCreationTime() const;

    /**
     * @brief Returns whether this asset is loaded automatically on repo load
     */
    bool getIsAutoLoaded() const { return isAutoLoaded; }

    /**
     * @brief Sets the display name of the asset
     *
     * @param name The new display name
     */
    void setDisplayName(const std::string& name);

    /**
     * @brief Sets the description of the asset
     *
     * @param description The new description
     */
    void setDescription(const std::string& description);

    /**
     * @brief Sets the creation time of the asset as a raw timestamp
     *
     * @param time The new creation time
     */
    void setCreationTime(std::uint64_t time);

    /**
     * @brief Sets whether this asset should be loaded automatically on repo load
     *
     * @param autoLoad True to load on repo load, false to defer until first read
     */
    void setIsAutoLoaded(bool autoLoad);

private:
    std::string displayName;
    std::string description;
    std::uint64_t creationTime;
    bool isAutoLoaded;
};

} // namespace as
} // namespace bl

#endif
