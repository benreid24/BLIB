#ifndef BLIB_SERIALIZATION_BINARY_SERIALIZABLEFIELD_HPP
#define BLIB_SERIALIZATION_BINARY_SERIALIZABLEFIELD_HPP

#include <BLIB/Logging.hpp>
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
    template<typename C, typename M>
    SerializableFieldBase(std::uint16_t id, SerializableObjectBase& owner, M C::*) {
        if (owner.fields.find(id) != owner.fields.end()) {
            BL_LOG_WARN << "Duplicate field id " << id << " encountered for object "
                        << typeid(C).name() << " member type " << typeid(M).name();
        }
        owner.fields[id] = this;
    }

    /**
     * @brief Returns the id of the field
     *
     */
    virtual std::uint16_t id() const = 0;

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
 * @tparam C The type of the class that the field belongs to
 * @tparam T The type of the data member
 * @ingroup Binary
 */
template<std::uint16_t Id, typename C, typename T>
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
     * @param member Pointer to the class member
     */
    SerializableField(SerializableObjectBase& owner, T C::*member)
    : SerializableFieldBase(Id, owner, member)
    , member(member) {}

    /**
     * @brief Returns the id of the field
     *
     */
    virtual std::uint16_t id() const override { return Id; }

    /**
     * @brief Serializes the object at the given address to the given stream
     *
     * @param stream The stream to serialize to
     * @param object The object to serialize
     * @return True on success, false on error
     */
    virtual bool serialize(OutputStream& stream, const void* obj) const override {
        const C& o = *static_cast<const C*>(obj);
        return Serializer<T>::serialize(stream, o.*member);
    }

    /**
     * @brief Deserializes the object at the given address to the given stream
     *
     * @param stream The stream to deserialize to
     * @param object The object to deserialize
     * @return True on success, false on error
     */
    virtual bool deserialize(InputStream& stream, void* object) const override {
        C& obj = *static_cast<C*>(object);
        return Serializer<T>::deserialize(stream, obj.*member);
    }

    /**
     * @brief Returns the serialized size of the field in the given object
     *
     * @param object The object to get the field value from
     * @return std::size_t The size of the field when serialized, in bytes
     */
    virtual std::size_t size(const void* object) const override {
        const C& obj = *static_cast<const C*>(object);
        return Serializer<T>::size(obj.*member);
    }

private:
    T C::*const member;
};

/**
 * @brief Descriptor class for a float member of a class to be serialized
 *
 * @tparam Id The id of the data member. Allows formats to change without invalidating existing data
 * @tparam C The type of the class that the field belongs to
 * @tparam F The type of the float data member
 * @ingroup Binary
 */
template<std::uint16_t Id, typename C>
struct SerializableFloatField
: public SerializableFieldBase
, private bl::util::NonCopyable {
    /**
     * @brief Removed
     *
     */
    SerializableFloatField() = delete;

    /**
     * @brief Construct the field with the object descriptor that it belongs to
     *
     * @param owner The serizable object descriptor that owns this field
     * @param member Pointer to the class member
     * @param precision Multiplier to use when converting the float values to and from std::int64_t
     */
    SerializableFloatField(SerializableObjectBase& owner, float C::*member, float precision)
    : SerializableFieldBase(Id, owner, member)
    , member(member)
    , M(precision) {}

    /**
     * @brief Returns the id of the field
     *
     */
    virtual std::uint16_t id() const override { return Id; }

    /**
     * @brief Serializes the object at the given address to the given stream
     *
     * @param stream The stream to serialize to
     * @param object The object to serialize
     * @return True on success, false on error
     */
    virtual bool serialize(OutputStream& stream, const void* obj) const override {
        const C& o = *static_cast<const C*>(obj);
        return Serializer<std::int64_t>::serialize(stream,
                                                   static_cast<std::int64_t>((o.*member * M)));
    }

    /**
     * @brief Deserializes the object at the given address to the given stream
     *
     * @param stream The stream to deserialize to
     * @param object The object to deserialize
     * @return True on success, false on error
     */
    virtual bool deserialize(InputStream& stream, void* object) const override {
        C& obj         = *static_cast<C*>(object);
        std::int64_t i = 0;
        if (!Serializer<std::int64_t>::deserialize(stream, i)) return false;
        obj.*member = static_cast<float>(i) / M;
        return true;
    }

    /**
     * @brief Returns the serialized size of the field in the given object
     *
     * @return std::size_t The size of the field when serialized, in bytes
     */
    virtual std::size_t size(const void*) const override {
        return Serializer<std::int64_t>::size(0);
    }

private:
    float C::*const member;
    const float M;
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
