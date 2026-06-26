#ifndef BLIB_ASSETS_BUNDLES_FILEMETADATA_HPP
#define BLIB_ASSETS_BUNDLES_FILEMETADATA_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>
#include <cstdint>
#include <string>

namespace bl
{
namespace as
{
namespace bdl
{
/**
 * @brief Metadata of an asset data file in a bundle
 *
 * @ingroup Assets
 */
struct FileMetadata {
    std::string path;
    std::uint64_t offset;
    std::uint64_t size;
};

} // namespace bdl
} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::bdl::FileMetadata> : public SerializableObjectBase {
    SerializableField<1, as::bdl::FileMetadata, std::string> path;
    SerializableField<2, as::bdl::FileMetadata, std::uint64_t> offset;
    SerializableField<3, as::bdl::FileMetadata, std::uint64_t> size;

    SerializableObject()
    : SerializableObjectBase("FileMetadata")
    , path("path", *this, &as::bdl::FileMetadata::path, SerializableFieldBase::Required{})
    , offset("offset", *this, &as::bdl::FileMetadata::offset, SerializableFieldBase::Required{})
    , size("size", *this, &as::bdl::FileMetadata::size, SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<as::bdl::FileMetadata> {
    inline static const auto spec = makeSpec<as::bdl::FileMetadata>(
        "FileMetadata", memberList(defineMember(1, "path", &as::bdl::FileMetadata::path),
                                   defineMember(2, "offset", &as::bdl::FileMetadata::offset),
                                   defineMember(3, "size", &as::bdl::FileMetadata::size)));
};
} // namespace refl

} // namespace bl

#endif
