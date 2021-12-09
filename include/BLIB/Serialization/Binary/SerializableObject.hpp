#ifndef BLIB_SERIALIZATION_SERIALIZABLEOBJECT_HPP
#define BLIB_SERIALIZATION_SERIALIZABLEOBJECT_HPP

#include <BLIB/Serialization/Binary/InputStream.hpp>
#include <BLIB/Serialization/Binary/OutputStream.hpp>
#include <unordered_map>

namespace bl
{
namespace serial
{
namespace binary
{
struct SerializableFieldBase;

/**
 * @brief Base class for object descriptors. Provides methods used by Serializer
 *
 * @ingroup Binary
 *
 */
class SerializableObjectBase {
public:
    /**
     * @brief Serializes the given object to the given stream
     *
     * @param stream The stream to serialize to
     * @param object The object to serialize
     * @return True on success, false on error
     */
    bool serialize(OutputStream& stream, const void* object) const;

    /**
     * @brief Deserializes from the stream into the given object
     *
     * @param stream The stream to read from
     * @param object The object to read into
     * @return True on success, false on error
     */
    bool deserialize(InputStream& stream, void* object) const;

    /**
     * @brief Returns the size of the object when serialized, in bytes
     *
     * @param object The object to serialize
     * @return std::size_t The serialized size of the object, in bytes
     */
    std::size_t size(const void* object) const;

private:
    std::unordered_map<std::uint16_t, const SerializableFieldBase*> fields;

    friend class SerializableFieldBase;
};

/**
 * @brief Base template for object descriptors. Specialize this for classes that you wish to
 *        serialize. If this is instantiated without a specialization a compiler error occurs
 *
 * @tparam T The type of object to describe serialization for
 */
template<typename T>
class SerializableObject : public SerializableObjectBase {
    template<typename U>
    struct MakeError : std::false_type {};

    static_assert(
        MakeError<T>::value,
        "Type does not have object descriptor. Specialize SerializableObject or Serializer.");
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
