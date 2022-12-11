#ifndef BLIB_SERIALIZATION_SERIALIZABLEFIELD_HPP
#define BLIB_SERIALIZATION_SERIALIZABLEFIELD_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Serialization/Binary/Serializer.hpp>
#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Serialization/JSON/Serializer.hpp>
#include <BLIB/Serialization/SerializableObject.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <memory>
#include <type_traits>

namespace bl
{
namespace serial
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
     * @brief Indicates that this field is optional
     *
     */
    struct Optional {};

    /**
     * @brief Indicates that this field is required
     *
     */
    struct Required {};

    /**
     * @brief Deserializes the json tree into the given object
     *
     * @param json The json tree to deserialize
     * @param object The object that owns the field to be read into
     * @return True on success, false on error
     */
    virtual bool deserializeJSON(const json::Value& json, void* object) const = 0;

    /**
     * @brief Serializes the field on the given object into a json value
     *
     * @param object The object that owns the field to be serialized
     * @return Value The serialized field
     */
    virtual json::Value serializeJSON(const void* object) const = 0;

    /**
     * @brief Serializes the given object directly into the given stream as json
     *
     * @param stream The stream to serialize into
     * @param object The object to serialize
     * @param tabSize Spaces to indent per level
     * @param currentIndent Current level of indentation in spaces
     * @return True on success, false on error
     */
    virtual bool serializeJsonStream(std::ostream& stream, const void* object, unsigned int tabSize,
                                     unsigned int currentIndent = 0) const = 0;

    /**
     * @brief Deserializes the given object directly from the given stream as json
     *
     * @param stream The stream to read json from
     * @param object The object to deserialize
     * @return True on success, false on error
     */
    virtual bool deserializeJsonStream(std::istream& stream, void* object) const = 0;

    /**
     * @brief Serializes the object at the given address to the given stream
     *
     * @param stream The stream to serialize to
     * @param object The object to serialize
     * @return True on success, false on error
     */
    virtual bool serializeBinary(binary::OutputStream& stream, const void* object) const = 0;

    /**
     * @brief Deserializes the object at the given address to the given stream
     *
     * @param stream The stream to deserialize to
     * @param object The object to deserialize
     * @return True on success, false on error
     */
    virtual bool deserializeBinary(binary::InputStream& stream, void* object) const = 0;

    /**
     * @brief Returns the serialized size of the field in the given object
     *
     * @param object The object to get the field value from
     * @return std::uint32_t The size of the field when serialized, in bytes
     */
    virtual std::uint32_t binarySize(const void* object) const = 0;

    /**
     * @brief Returns the id of the field
     *
     */
    std::uint16_t id() const { return i; }

    /**
     * @brief Get the name of this field in json
     *
     */
    const std::string& name() const { return nm; }

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
    /**
     * @brief Construct a new Serializable Field Base object
     *
     * @tparam C The parent object type
     * @tparam M The field type
     * @param id The id of the field
     * @param name The name of the field
     * @param owner The parent object descriptor
     * @param opt Whether or not the field is optional
     */
    template<typename C, typename M>
    SerializableFieldBase(std::uint16_t id, const std::string& name, SerializableObjectBase& owner,
                          M C::*, bool opt)
    : i(id)
    , nm(name)
    , opt(opt) {
        if (owner.fieldsJson.find(name) != owner.fieldsJson.end()) {
            BL_LOG_WARN << "Duplicate field name " << name << " encountered for object "
                        << typeid(C).name() << " member type " << typeid(M).name();
        }
        if (owner.fieldsBinary.find(id) != owner.fieldsBinary.end()) {
            BL_LOG_WARN << "Duplicate field id " << id << " encountered for object "
                        << typeid(C).name() << " member type " << typeid(M).name();
        }
        owner.fieldsJson[name] = this;
        owner.fieldsBinary[id] = this;

        owner.sortedFields.emplace(
            std::upper_bound(
                owner.sortedFields.begin(),
                owner.sortedFields.end(),
                id,
                [](std::uint16_t val,
                   const std::pair<std::uint16_t, const SerializableFieldBase*>& elem) {
                    return val < elem.first;
                }),
            id,
            this);
    }

private:
    const std::uint16_t i;
    const std::string nm;
    const bool opt;
};

/**
 * @brief Represents a field in a class that can be serialized. Serializable objects should inherit
 *        from SerializableObject and make their data fields of type SerializableField.
 *        Serialization is builtin for primitive json types, vectors, maps, and nested
 *        SerializableObject members. Specializations of Serializer may be created to support other
 *        types which are not easily represented as Serializable objects
 *
 * @tparam Id Integer id of this field
 * @tparam C The object type the field belongs to
 * @tparam T The underlying type of the field
 * @tparam AllowDefault True to allow defaulting, false to disable
 * @ingroup JSON
 */
template<std::uint16_t Id, typename C, typename T, bool AllowDefault = true>
class SerializableField : public SerializableFieldBase {
public:
    /**
     * @brief Create a new required field descriptor
     *
     * @param name Name to use when serializing and deserializing
     * @param owner The parent Serializable object to register with
     * @param member Pointer to the member to serialize
     */
    SerializableField(const std::string& name, SerializableObjectBase& owner, T C::*member,
                      SerializableFieldBase::Required&&);

    /**
     * @brief Create a new optional field descriptor
     *
     * @param name Name to use when serializing and deserializing
     * @param owner The parent Serializable object to register with
     * @param member Pointer to the member to serialize
     */
    SerializableField(const std::string& name, SerializableObjectBase& owner, T C::*member,
                      SerializableFieldBase::Optional&&);

    /**
     * @brief Updates the value of this field from the json data
     *
     * @param json JSON value to deserialize from
     * @param object Address of the object being deserialized
     * @return True if successfully deserialized, false on error
     */
    virtual bool deserializeJSON(const json::Value& json, void* object) const override;

    /**
     * @brief Serializes the held value of this field to JSON
     *
     * @param object Address of the object being serialized
     * @return Value JSON value of this field
     */
    virtual json::Value serializeJSON(const void* object) const override;

    /**
     * @brief Serializes the given object directly into the given stream as json
     *
     * @param stream The stream to serialize into
     * @param object The object to serialize
     * @param tabSize Spaces to indent per level
     * @param currentIndent Current level of indentation in spaces
     * @return True on success, false on error
     */
    virtual bool serializeJsonStream(std::ostream& stream, const void* object, unsigned int tabSize,
                                     unsigned int currentIndent = 0) const override;

    /**
     * @brief Deserializes the given object directly from the given stream as json
     *
     * @param stream The stream to read json from
     * @param object The object to deserialize
     * @return True on success, false on error
     */
    virtual bool deserializeJsonStream(std::istream& stream, void* object) const override;

    /**
     * @brief Serializes the object at the given address to the given stream
     *
     * @param stream The stream to serialize to
     * @param object The object to serialize
     * @return True on success, false on error
     */
    virtual bool serializeBinary(binary::OutputStream& stream, const void* object) const override;

    /**
     * @brief Deserializes the object at the given address to the given stream
     *
     * @param stream The stream to deserialize to
     * @param object The object to deserialize
     * @return True on success, false on error
     */
    virtual bool deserializeBinary(binary::InputStream& stream, void* object) const override;

    /**
     * @brief Returns the serialized size of the field in the given object
     *
     * @param object The object to get the field value from
     * @return std::uint32_t The size of the field when serialized, in bytes
     */
    virtual std::uint32_t binarySize(const void* object) const override;

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

template<std::uint16_t Id, typename C, typename T, bool AD>
SerializableField<Id, C, T, AD>::SerializableField(const std::string& name,
                                                   SerializableObjectBase& owner, T C::*member,
                                                   SerializableFieldBase::Required&&)
: SerializableFieldBase(Id, name, owner, member, false)
, member(member) {}

template<std::uint16_t Id, typename C, typename T, bool AD>
SerializableField<Id, C, T, AD>::SerializableField(const std::string& name,
                                                   SerializableObjectBase& owner, T C::*member,
                                                   SerializableFieldBase::Optional&&)
: SerializableFieldBase(Id, name, owner, member, true)
, member(member) {}

template<std::uint16_t Id, typename C, typename T, bool AD>
bool SerializableField<Id, C, T, AD>::deserializeJSON(const json::Value& v, void* obj) const {
    C& o = *static_cast<C*>(obj);
    return json::Serializer<T>::deserialize(o.*member, v);
}

template<std::uint16_t Id, typename C, typename T, bool AD>
json::Value SerializableField<Id, C, T, AD>::serializeJSON(const void* obj) const {
    const C& o = *static_cast<const C*>(obj);
    return json::Serializer<T>::serialize(o.*member);
}

template<std::uint16_t Id, typename C, typename T, bool AD>
bool SerializableField<Id, C, T, AD>::deserializeJsonStream(std::istream& s, void* obj) const {
    C& o = *static_cast<C*>(obj);
    return json::Serializer<T>::deserializeStream(s, o.*member);
}

template<std::uint16_t Id, typename C, typename T, bool AD>
bool SerializableField<Id, C, T, AD>::serializeJsonStream(std::ostream& s, const void* obj,
                                                          unsigned int tabSize,
                                                          unsigned int currentIndent) const {
    const C& o = *static_cast<const C*>(obj);
    return json::Serializer<T>::serializeStream(s, o.*member, tabSize, currentIndent);
}

template<std::uint16_t Id, typename C, typename T, bool AD>
bool SerializableField<Id, C, T, AD>::serializeBinary(binary::OutputStream& out,
                                                      const void* obj) const {
    const C& o = *static_cast<const C*>(obj);
    return binary::Serializer<T>::serialize(out, o.*member);
}

template<std::uint16_t Id, typename C, typename T, bool AD>
bool SerializableField<Id, C, T, AD>::deserializeBinary(binary::InputStream& in, void* obj) const {
    C& o = *static_cast<C*>(obj);
    return binary::Serializer<T>::deserialize(in, o.*member);
}

template<std::uint16_t Id, typename C, typename T, bool AD>
std::uint32_t SerializableField<Id, C, T, AD>::binarySize(const void* obj) const {
    const C& o = *static_cast<const C*>(obj);
    return binary::Serializer<T>::size(o.*member);
}

template<std::uint16_t Id, typename C, typename T, bool AD>
void SerializableField<Id, C, T, AD>::makeDefault(void* obj) const {
    // TODO - remove the AD workaround for copy assignment check
    if constexpr (AD) {
        if (defVal) {
            C& o = *static_cast<C*>(obj);
            if constexpr (!std::is_array_v<T>) { o.*member = *defVal; }
            else {
                for (unsigned int i = 0; i < sizeof(o.*member); ++i) {
                    (o.*member)[i] = (*defVal)[i];
                }
            }
        }
    }
    else {
        (void)obj; // prevent warning
    }
}

template<std::uint16_t Id, typename C, typename T, bool AD>
void SerializableField<Id, C, T, AD>::setDefault(T&& d) {
    defVal = std::make_unique<T>(std::forward<T>(d));
}

} // namespace serial
} // namespace bl

#endif
