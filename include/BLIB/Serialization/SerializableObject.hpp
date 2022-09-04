#ifndef BLIB_SERIALIZATION_SERIALIZABLEOBJECT_HPP
#define BLIB_SERIALIZATION_SERIALIZABLEOBJECT_HPP

#include <BLIB/Serialization/Binary/InputStream.hpp>
#include <BLIB/Serialization/Binary/OutputStream.hpp>
#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace serial
{
class SerializableFieldBase;

/**
 * @brief Base class for all serializable objects
 *
 */
struct SerializableObjectBase : private util::NonCopyable {
    /**
     * @brief Deserializes the given json into the fields of this object
     *
     * @param value The json data to populate fields with
     * @param object The object to read into
     * @return True on success, false on error
     */
    bool deserializeJSON(const json::Group& value, void* object) const;

    /**
     * @brief Serializes this object and its fields into the returned json data
     *
     * @param object The object to serialize
     * @return Group json data containing this object and its fields
     */
    json::Group serializeJSON(const void* object) const;

    /**
     * @brief Serializes the given object to the given stream
     *
     * @param stream The stream to serialize to
     * @param object The object to serialize
     * @return True on success, false on error
     */
    bool serializeBinary(binary::OutputStream& stream, const void* object) const;

    /**
     * @brief Deserializes from the stream into the given object
     *
     * @param stream The stream to read from
     * @param object The object to read into
     * @return True on success, false on error
     */
    bool deserializeBinary(binary::InputStream& stream, void* object) const;

    /**
     * @brief Returns the size of the object when serialized, in bytes
     *
     * @param object The object to serialize
     * @return std::size_t The serialized size of the object, in bytes
     */
    std::size_t binarySize(const void* object) const;

    /**
     * @brief Returns the serializable object descriptor for the given type
     *
     * @tparam T The type to get the descriptor for
     * @return SerializableObjectBase& The object descriptor
     */
    template<typename T>
    static SerializableObjectBase& get();

protected:
    SerializableObjectBase() = default;

private:
    std::unordered_map<std::uint16_t, const SerializableFieldBase*> fieldsBinary;
    std::unordered_map<std::string, const SerializableFieldBase*> fieldsJson;

    friend class SerializableFieldBase;
};

/**
 * @brief Base class for user defined classes meant to be serialized to and from JSON. Data
 * members of serializable objects should be of type SerializableField. JSON native types,
 * vector, unordered_map, and nested SerializableObject members are supported out of the box.
 * For user defined types as fields they must either extend SerializableObject themselves, or a
 * specialization of Serializer must be provided for the desired type. SerializableObjects that
 * will be nested must provide a copy constructor that registers each contained field.
 *
 * @tparam T The type of object to serialize
 * @ingroup JSON
 */
template<typename T>
class SerializableObject : public SerializableObjectBase {
    template<typename U>
    struct MakeError : std::false_type {};

    static_assert(
        MakeError<T>::value,
        "Type does not have object descriptor. Specialize SerializableObject or Serializer.");
};

template<typename T>
SerializableObjectBase& SerializableObjectBase::get() {
    static SerializableObject<T> descriptor;
    return descriptor;
}

} // namespace serial
} // namespace bl

#endif
