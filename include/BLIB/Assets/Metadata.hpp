#ifndef BLIB_ASSETS_METADATA_HPP
#define BLIB_ASSETS_METADATA_HPP

#include <BLIB/Serialization.hpp>
#include <chrono>
#include <cstdint>
#include <string>

namespace bl
{
namespace as
{
class Asset;

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
     * @brief Returns the path of the asset
     */
    const std::string& getPath() const { return path; }

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
     * @brief Sets the path of the asset
     *
     * @param path The new path
     */
    void setPath(const std::string& path);

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
    std::string path;
    std::uint64_t creationTime;
    bool isAutoLoaded;
    Asset* owner;

    friend class Asset;
    friend struct serial::SerializableObject<as::Metadata>;
};

} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::Metadata> : public SerializableObjectBase {
    SerializableField<1, as::Metadata, std::string> displayName;
    SerializableField<2, as::Metadata, std::string> description;
    SerializableField<3, as::Metadata, std::string> path;
    SerializableField<4, as::Metadata, std::uint64_t> creationTime;
    SerializableField<5, as::Metadata, bool> isAutoLoaded;

    SerializableObject()
    : SerializableObjectBase("AssetMetadata")
    , displayName("displayName", *this, &as::Metadata::displayName,
                  SerializableFieldBase::Required{})
    , description("description", *this, &as::Metadata::description,
                  SerializableFieldBase::Required{})
    , path("path", *this, &as::Metadata::path, SerializableFieldBase::Required{})
    , creationTime("creationTime", *this, &as::Metadata::creationTime,
                   SerializableFieldBase::Required{})
    , isAutoLoaded("isAutoLoaded", *this, &as::Metadata::isAutoLoaded,
                   SerializableFieldBase::Required{}) {}
};
} // namespace serial

} // namespace bl

#endif
