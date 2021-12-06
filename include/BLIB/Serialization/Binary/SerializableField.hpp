#ifndef BLIB_SERIALIZATION_BINARY_SERIALIZABLEFIELD_HPP
#define BLIB_SERIALIZATION_BINARY_SERIALIZABLEFIELD_HPP

#include <BLIB/Serialization/Binary/SerializableObject.hpp>
#include <BLIB/Serialization/Binary/Serializer.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <cstdint>

namespace bl
{
namespace serial
{
namespace binary
{
/**
 * @brief Base class for representing serializable class members. Do not use directly
 *
 * @ingroup Binary
 *
 */
struct SerializableFieldBase {
    /**
     * @brief Construct the field with the object descriptor that it belongs to
     *
     * @param id The id of the field
     * @param owner The serizable object descriptor that owns this field
     */
    SerializableFieldBase(std::uint16_t id, SerializableObjectBase& owner) {
        owner.fields[id] = this;
    }

    /**
     * @brief Returns the id of the field
     *
     */
    virtual std::uint16_t id() const = 0;

    /**
     * @brief Returns the offset of the field
     *
     */
    virtual std::size_t offset() const = 0;

    /**
     * @brief Serializes the object at the given address to the given stream
     *
     * @param stream The stream to serialize to
     * @param object The object to serialize
     * @return True on success, false on error
     */
    virtual bool serialize(OutputStream& stream, const void* object) const = 0;

    /**
     * @brief Deserializes the object at the given address to the given stream
     *
     * @param stream The stream to deserialize to
     * @param object The object to deserialize
     * @return True on success, false on error
     */
    virtual bool deserialize(InputStream& stream, void* object) const = 0;

    /**
     * @brief Returns the serialized size of the field in the given object
     *
     * @param object The object to get the field value from
     * @return std::size_t The size of the field when serialized, in bytes
     */
    virtual std::size_t size(const void* object) const = 0;
};

/**
 * @brief Descriptor class for a data member of a class to be serialized
 *
 * @tparam Id The id of the data member. Allows formats to change without invalidating existing data
 * @tparam T The type of the data member
 * @tparam Offset The offset of the data member in it's class
 * @ingroup Binary
 */
template<std::uint16_t Id, typename T, std::size_t Offset>
struct SerializableField
: public SerializableFieldBase
, private bl::util::NonCopyable {
    /**
     * @brief Removed
     *
     */
    SerializableField() = delete;

    /**
     * @brief Construct the field with the object descriptor that it belongs to
     *
     * @param owner The serizable object descriptor that owns this field
     */
    SerializableField(SerializableObjectBase& owner)
    : SerializableFieldBase(Id, owner) {}

    /**
     * @brief Returns the id of the field
     *
     */
    virtual std::uint16_t id() const override { return Id; }

    /**
     * @brief Returns the offset of the field
     *
     */
    virtual std::size_t offset() const override { return Offset; }

    /**
     * @brief Serializes the object at the given address to the given stream
     *
     * @param stream The stream to serialize to
     * @param object The object to serialize
     * @return True on success, false on error
     */
    virtual bool serialize(OutputStream& stream, const void* obj) const override {
        const char* addr = static_cast<const char*>(obj) + Offset;
        const T* field   = static_cast<const T*>(static_cast<const void*>(addr));
        return Serializer<T>::serialize(stream, *field);
    }

    /**
     * @brief Deserializes the object at the given address to the given stream
     *
     * @param stream The stream to deserialize to
     * @param object The object to deserialize
     * @return True on success, false on error
     */
    virtual bool deserialize(InputStream& stream, void* object) const override {
        char* addr = static_cast<char*>(object) + Offset;
        T* field   = static_cast<T*>(static_cast<void*>(addr));
        return Serializer<T>::deserialize(stream, *field);
    }

    /**
     * @brief Returns the serialized size of the field in the given object
     *
     * @param object The object to get the field value from
     * @return std::size_t The size of the field when serialized, in bytes
     */
    virtual std::size_t size(const void* object) const override {
        const char* addr = static_cast<const char*>(object) + Offset;
        const T* field   = static_cast<const T*>(static_cast<const void*>(addr));
        return Serializer<T>::size(*field);
    }
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
