#ifndef BLIB_SERIALIZATION_JSON_SERIALIZABLEFIELD_HPP
#define BLIB_SERIALIZATION_JSON_SERIALIZABLEFIELD_HPP

#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Serialization/JSON/SerializableObject.hpp>
#include <BLIB/Serialization/JSON/Serializer.hpp>
#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace serial
{
namespace json
{
/**
 * @brief Base class for SerializableField objects. Not to be used directly
 *
 * @ingroup JSON
 *
 */
class SerializableFieldBase : private util::NonCopyable {
public:
    /**
     * @brief Deserializes the json tree into the given object
     *
     * @param json The json tree to deserialize
     * @param object The object that owns the field to be read into
     * @return True on success, false on error
     */
    virtual bool deserialize(const Value& json, void* object) const = 0;

    /**
     * @brief Serializes the field on the given object into a json value
     *
     * @param object The object that owns the field to be serialized
     * @return Value The serialized field
     */
    virtual Value serialize(const void* object) const = 0;

    /**
     * @brief Get the name of this field in json
     *
     */
    const std::string& getName() const { return name; }

protected:
    SerializableFieldBase(const std::string& name, SerializableObjectBase& owner)
    : name(name) {
        owner.fields[name] = this;
    }

private:
    const std::string name;
};

/**
 * @brief Represents a field in a class that can be serialized. Serializable objects should inherit
 *        from SerializableObject and make their data fields of type SerializableField.
 *        Serialization is builtin for primitive json types, vectors, maps, and nested
 *        SerializableObject members. Specializations of Serializer may be created to support other
 *        types which are not easily represented as Serializable objects
 *
 * @tparam T The underlying type of the field
 * @tparam Offset The offset of the field in bytes
 * @ingroup JSON
 */
template<typename T, std::size_t Offset>
class SerializableField : public SerializableFieldBase {
public:
    /**
     * @brief Assigns the field a name to be used in JSON and registers itself
     *
     * @param name Name to use when serializing and deserializing
     * @param owner The parent Serializable object to register with
     */
    SerializableField(const std::string& name, SerializableObjectBase& owner);

    /**
     * @brief Updates the value of this field from the json data
     *
     * @param json JSON value to deserialize from
     * @param object Address of the object being deserialized
     * @return True if successfully deserialized, false on error
     */
    virtual bool deserialize(const Value& json, void* object) const override;

    /**
     * @brief Serializes the held value of this field to JSON
     *
     * @param object Address of the object being serialized
     * @return Value JSON value of this field
     */
    virtual Value serialize(const void* object) const override;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T, std::size_t Offset>
SerializableField<T, Offset>::SerializableField(const std::string& name,
                                                SerializableObjectBase& owner)
: SerializableFieldBase(name, owner) {}

template<typename T, std::size_t Offset>
bool SerializableField<T, Offset>::deserialize(const Value& v, void* obj) const {
    char* addr = static_cast<char*>(obj) + Offset;
    T* field   = static_cast<T*>(static_cast<void*>(addr));
    return Serializer<T>::deserialize(*field, v);
}

template<typename T, std::size_t Offset>
Value SerializableField<T, Offset>::serialize(const void* obj) const {
    const char* addr = static_cast<const char*>(obj) + Offset;
    const T* field   = static_cast<const T*>(static_cast<const void*>(addr));
    return Serializer<T>::serialize(*field);
}

} // namespace json
} // namespace serial
} // namespace bl

#endif
