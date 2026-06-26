#ifndef BLIB_ASSETS_BUNDLES_BUNDLADATA_HPP
#define BLIB_ASSETS_BUNDLES_BUNDLADATA_HPP

#include <BLIB/Assets/Bundles/FileMetadata.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Util/UUID.hpp>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace as
{
namespace bdl
{
/**
 * @brief Metadata for a single bundle file
 *
 * @ingroup Assets
 */
struct BundleData {
    std::uint32_t headerSize;
    util::UUID uuid;
    std::unordered_map<util::UUID, std::unordered_map<std::string, FileMetadata>> assetFileManifest;
    std::vector<util::UUID> autoLoadAssets;
    std::vector<char> data;

    /**
     * @brief Creates an empty bundle data
     */
    BundleData();

    /**
     * @brief Flushes the bundle data to disk
     *
     * @param base The root path where bundles are written
     * @return True if the bundle was able to be flushed, false on error
     */
    bool flush(const std::string& base);

    /**
     * @brief Resets the bundle data to start fresh for a new bundle
     */
    void reset();
};

} // namespace bdl
} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::bdl::BundleData> : public SerializableObjectBase {
    SerializableField<1, as::bdl::BundleData, std::uint32_t> headerSize;
    SerializableField<2, as::bdl::BundleData, util::UUID> uuid;
    SerializableField<
        3, as::bdl::BundleData,
        std::unordered_map<util::UUID, std::unordered_map<std::string, as::bdl::FileMetadata>>>
        assetFileManifest;
    SerializableField<4, as::bdl::BundleData, std::vector<util::UUID>> autoLoadAssets;
    SerializableField<5, as::bdl::BundleData, std::vector<char>> data;

    SerializableObject()
    : SerializableObjectBase("BundleData")
    , headerSize("headerSize", *this, &as::bdl::BundleData::headerSize,
                 SerializableFieldBase::Required{})
    , uuid("uuid", *this, &as::bdl::BundleData::uuid, SerializableFieldBase::Required{})
    , assetFileManifest("assetFileManifest", *this, &as::bdl::BundleData::assetFileManifest,
                        SerializableFieldBase::Required{})
    , autoLoadAssets("autoLoadAssets", *this, &as::bdl::BundleData::autoLoadAssets,
                     SerializableFieldBase::Required{})
    , data("data", *this, &as::bdl::BundleData::data, SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<as::bdl::BundleData> {
    inline static const auto spec = makeSpec<as::bdl::BundleData>(
        "BundleData",
        memberList(defineMember(1, "headerSize", &as::bdl::BundleData::headerSize),
                   defineMember(2, "uuid", &as::bdl::BundleData::uuid),
                   defineMember(3, "assetFileManifest", &as::bdl::BundleData::assetFileManifest),
                   defineMember(4, "autoLoadAssets", &as::bdl::BundleData::autoLoadAssets),
                   defineMember(5, "data", &as::bdl::BundleData::data)));
};
} // namespace refl

} // namespace bl

#endif
