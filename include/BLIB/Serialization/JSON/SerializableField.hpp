#ifndef BLIB_SERIALIZATION_JSON_SERIALIZABLEFIELD_HPP
#define BLIB_SERIALIZATION_JSON_SERIALIZABLEFIELD_HPP

#include <BLIB/Logging.hpp>
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
    template<typename C, typename M>
    SerializableFieldBase(const std::string& name, SerializableObjectBase& owner, M C::*)
    : name(name) {
        if (owner.fields.find(name) != owner.fields.end()) {
            BL_LOG_WARN << "Duplicate field name " << name << " encountered for object "
                        << typeid(C).name() << " member type " << typeid(M).name();
        }
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
 * @tparam C The object type the field belongs to
 * @tparam T The underlying type of the field
 * @ingroup JSON
 */
template<typename C, typename T>
class SerializableField : public SerializableFieldBase {
public:
    /**
     * @brief Assigns the field a name to be used in JSON and registers itself
     *
     * @param name Name to use when serializing and deserializing
     * @param owner The parent Serializable object to register with
     * @param member Pointer to the member to serialize
     */
    SerializableField(const std::string& name, SerializableObjectBase& owner, T C::*member);

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

private:
    T C::*const member;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename C, typename T>
SerializableField<C, T>::SerializableField(const std::string& name, SerializableObjectBase& owner,
                                           T C::*member)
: SerializableFieldBase(name, owner, member)
, member(member) {}

template<typename C, typename T>
bool SerializableField<C, T>::deserialize(const Value& v, void* obj) const {
    C& o = *static_cast<C*>(obj);
    return Serializer<T>::deserialize(o.*member, v);
}

template<typename C, typename T>
Value SerializableField<C, T>::serialize(const void* obj) const {
    const C& o = *static_cast<const C*>(obj);
    return Serializer<T>::serialize(o.*member);
}

} // namespace json
} // namespace serial
} // namespace bl

#endif
