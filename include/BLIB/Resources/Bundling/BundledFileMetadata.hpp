#ifndef BLIB_RESOURCES_BUNDLE_BUNDLEDFILEMETADATA_HPP
#define BLIB_RESOURCES_BUNDLE_BUNDLEDFILEMETADATA_HPP

#include <BLIB/Serialization.hpp>
#include <cstdint>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief Basic struct containing metadata about a file in a bundle
 *
 * @ingroup Bundling
 *
 */
struct BundledFileMetadata {
    /// @brief The offset of the file in the bundle, relative to the start of the file
    std::int64_t offset;

    /// @brief The number of bytes the file has
    std::int64_t length;

    /**
     * @brief Construct an empty Bundled File Metadata
     *
     */
    BundledFileMetadata()
    : offset(0)
    , length(0) {}

    /**
     * @brief Construct a new Bundled File Metadata from the given information
     *
     * @param offset The offset from the start of the bundle file
     * @param length The number of bytes in the file
     */
    BundledFileMetadata(std::int64_t offset, std::int64_t length)
    : offset(offset)
    , length(length) {}
};

} // namespace bundle
} // namespace resource

namespace serial
{
template<>
struct SerializableObject<resource::bundle::BundledFileMetadata> : public SerializableObjectBase {
    using T = resource::bundle::BundledFileMetadata;

    SerializableField<1, T, std::int64_t> offset;
    SerializableField<2, T, std::int64_t> length;

    SerializableObject()
    : SerializableObjectBase("BundleHeader")
    , offset("offset", *this, &T::offset, SerializableFieldBase::Required{})
    , length("length", *this, &T::length, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
