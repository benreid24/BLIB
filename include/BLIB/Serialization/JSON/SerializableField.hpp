#ifndef BLIB_SERIALIZATION_JSON_SERIALIZABLEFIELD_HPP
#define BLIB_SERIALIZATION_JSON_SERIALIZABLEFIELD_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Serialization/JSON/SerializableObject.hpp>
#include <BLIB/Serialization/JSON/Serializer.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <memory>
#include <type_traits>

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

    /**
     * @brief Whether or not this field is optional
     *
     */
    bool optional() const { return opt; }

    /**
     * @brief Initialize the object to the given default value, if any
     *
     * @param obj The object to default
     */
    virtual void makeDefault(void* obj) const = 0;

protected:
    template<typename C, typename M>
    SerializableFieldBase(const std::string& name, SerializableObjectBase& owner, M C::*, bool opt)
    : name(name)
    , opt(opt) {
        if (owner.fields.find(name) != owner.fields.end()) {
            BL_LOG_WARN << "Duplicate field name " << name << " encountered for object "
                        << typeid(C).name() << " member type " << typeid(M).name();
        }
        owner.fields[name] = this;
    }

private:
    const std::string name;
    const bool opt;
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
 * @tparam Optional Whether or not this field is optional. Overrides strict/relaxed setting
 * @ingroup JSON
 */
template<typename C, typename T, bool Optional = false>
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

    /**
     * @brief Initialize the object to the given default value, if any
     *
     * @param obj The object to default
     */
    virtual void makeDefault(void* object) const override;

    /**
     * @brief Set the default value for deserialization if the field is not present
     *
     * @param defaultValue The value to default to
     */
    void setDefault(T&& defaultValue);

private:
    T C::*const member;
    std::unique_ptr<T> defVal;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename C, typename T, bool O>
SerializableField<C, T, O>::SerializableField(const std::string& name,
                                              SerializableObjectBase& owner, T C::*member)
: SerializableFieldBase(name, owner, member, O)
, member(member) {}

template<typename C, typename T, bool O>
bool SerializableField<C, T, O>::deserialize(const Value& v, void* obj) const {
    C& o = *static_cast<C*>(obj);
    return Serializer<T>::deserialize(o.*member, v);
}

template<typename C, typename T, bool O>
Value SerializableField<C, T, O>::serialize(const void* obj) const {
    const C& o = *static_cast<const C*>(obj);
    return Serializer<T>::serialize(o.*member);
}

template<typename C, typename T, bool O>
void SerializableField<C, T, O>::makeDefault(void* obj) const {
    if (defVal) {
        C& o = *static_cast<C*>(obj);
        if constexpr (!std::is_array_v<T>) { o.*member = *defVal; }
        else {
            for (unsigned int i = 0; i < sizeof(o.*member); ++i) { (o.*member)[i] = (*defVal)[i]; }
        }
    }
}

template<typename C, typename T, bool O>
void SerializableField<C, T, O>::setDefault(T&& d) {
    defVal.reset(new T(std::forward<T>(d)));
}

} // namespace json
} // namespace serial
} // namespace bl

#endif
