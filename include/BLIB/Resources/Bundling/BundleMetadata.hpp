#ifndef BLIB_RESOURCES_BUNDLING_BUNDLEMETADATA_HPP
#define BLIB_RESOURCES_BUNDLING_BUNDLEMETADATA_HPP

#include <BLIB/Resources/Bundling/BundledFileMetadata.hpp>
#include <BLIB/Serialization.hpp>
#include <string>
#include <unordered_map>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief Class containing a file manifest for an individual bundle. Essentially a wrapped map of
 *        string -> BundledFileMetadata
 *
 * @ingroup Bundling
 */
class BundleMetadata {
public:
    /**
     * @brief Construct an empty Bundle Metadata object
     *
     */
    BundleMetadata() = default;

    /**
     * @brief Adds information for the given file to the bundle manifest
     *
     * @param path The original resource path
     * @param info Information locating the resource in the bundle file
     */
    void addFileInfo(const std::string& path, const BundledFileMetadata& info);

    /**
     * @brief Get the information locating a specific file in the bundle
     *
     * @param path The original resource path
     * @return BundledFileMetadata The info locating the file, if found
     */
    BundledFileMetadata getFileInfo(const std::string& path) const;

    /**
     * @brief Saves the bundle metadata to the given stream
     *
     * @param stream The stream to save to
     * @return True if the data was saved, false otherwise
     */
    bool save(std::ostream& stream) const;

    /**
     * @brief Loads the bundle metadata from the given stream
     *
     * @param stream The stream to load from
     * @return True if the bundle could be loaded, false otherwise
     */
    bool load(std::istream& stream);

private:
    std::unordered_map<std::string, BundledFileMetadata> manifest;

    friend struct serial::SerializableObject<BundleMetadata>;
};

} // namespace bundle
} // namespace resource

namespace serial
{
template<>
struct SerializableObject<resource::bundle::BundleMetadata> : public SerializableObjectBase {
    using T  = resource::bundle::BundleMetadata;
    using MD = resource::bundle::BundledFileMetadata;

    SerializableField<1, T, std::unordered_map<std::string, MD>> manifest;

    SerializableObject()
    : SerializableObjectBase("BundleMetadata")
    , manifest("manifest", *this, &T::manifest, SerializableFieldBase::Required{}) {}
};
} // namespace serial
} // namespace bl

#endif
