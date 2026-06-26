#ifndef BLIB_ASSETS_METADATA_HPP
#define BLIB_ASSETS_METADATA_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
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
     * @brief Basic struct for storing source file info
     */
    struct SourceFileInfo {
        std::string path;
        std::time_t lastModified;
    };

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

    /**
     * @brief Initializes the source file info for this asset.
     *
     * @param path The path of the source file this asset was created from
     */
    void setSourceFileInfo(const std::string& path);

    /**
     * @brief Queries and updates the last modified time of the source file
     */
    void updateSourceFileModifiedTime();

    /**
     * @brief Returns a pointer to the source file info. Nullptr if none is present
     */
    const SourceFileInfo* getSourceFileInfo() const {
        return sourceFileInfo ? &*sourceFileInfo : nullptr;
    }

    /**
     * @brief Returns whether this metadata has source file info
     */
    bool hasSourceFileInfo() const { return sourceFileInfo.has_value(); }

private:
    std::string displayName;
    std::string description;
    std::string path;
    std::uint64_t creationTime;
    bool isAutoLoaded;
    std::optional<SourceFileInfo> sourceFileInfo;
    Asset* owner;

    friend class Asset;
    friend struct serial::SerializableObject<as::Metadata>;
    friend struct refl::ReflectedObject<as::Metadata>;
};

} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::Metadata::SourceFileInfo> : public SerializableObjectBase {
    SerializableField<1, as::Metadata::SourceFileInfo, std::string> path;
    SerializableField<2, as::Metadata::SourceFileInfo, std::time_t> lastModified;

    SerializableObject()
    : SerializableObjectBase("SourceFileInfo")
    , path("path", *this, &as::Metadata::SourceFileInfo::path, SerializableFieldBase::Required{})
    , lastModified("lastModified", *this, &as::Metadata::SourceFileInfo::lastModified,
                   SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<as::Metadata> : public SerializableObjectBase {
    SerializableField<1, as::Metadata, std::string> displayName;
    SerializableField<2, as::Metadata, std::string> description;
    SerializableField<3, as::Metadata, std::string> path;
    SerializableField<4, as::Metadata, std::uint64_t> creationTime;
    SerializableField<5, as::Metadata, bool> isAutoLoaded;
    SerializableField<6, as::Metadata, std::optional<as::Metadata::SourceFileInfo>> sourceFileInfo;

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
                   SerializableFieldBase::Required{})
    , sourceFileInfo("sourceFileInfo", *this, &as::Metadata::sourceFileInfo,
                     SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<as::Metadata::SourceFileInfo> {
    inline static const auto spec = makeSpec<as::Metadata::SourceFileInfo>(
        "SourceFileInfo",
        memberList(defineMember(1, "path", &as::Metadata::SourceFileInfo::path),
                   defineMember(2, "lastModified", &as::Metadata::SourceFileInfo::lastModified)));
};

template<>
struct ReflectedObject<as::Metadata> {
    inline static const auto spec = makeSpec<as::Metadata>(
        "AssetMetadata",
        memberList(defineMember(1, "displayName", &as::Metadata::displayName),
                   defineMember(2, "description", &as::Metadata::description),
                   defineMember(3, "path", &as::Metadata::path),
                   defineMember(4, "creationTime", &as::Metadata::creationTime),
                   defineMember(5, "isAutoLoaded", &as::Metadata::isAutoLoaded),
                   defineMember(6, "sourceFileInfo", &as::Metadata::sourceFileInfo)));
};
} // namespace refl

} // namespace bl

#endif
