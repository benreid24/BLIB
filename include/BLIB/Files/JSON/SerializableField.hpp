#ifndef BLIB_FILES_JSON_SERIALIZABLEFIELD_HPP
#define BLIB_FILES_JSON_SERIALIZABLEFIELD_HPP

#include <BLIB/Files/JSON/JSON.hpp>
#include <BLIB/Files/JSON/SerializableObject.hpp>
#include <BLIB/Files/JSON/Serializer.hpp>

namespace bl
{
namespace json
{
/**
 * @brief Base class for SerializableField objects. Not to be used directly
 *
 * @ingroup JSON
 *
 */
class SerializableFieldBase {
public:
    virtual bool deserialize(const Value& json) = 0;

    virtual Value serialize() const = 0;

    const std::string& getName() const;

    SerializableFieldBase(const SerializableFieldBase& copy) = delete;

    SerializableFieldBase(SerializableFieldBase&& copy) = delete;

protected:
    SerializableFieldBase(const std::string& name, SerializableObject& owner);

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
 * @ingroup JSON
 */
template<typename T>
class SerializableField : public SerializableFieldBase {
public:
    /**
     * @brief Assigns the field a name to be used in JSON and registers itself
     *
     * @param name Name to use when serializing and deserializing
     * @param owner The parent Serializable object to register with
     */
    SerializableField(const std::string& name, SerializableObject& owner);

    /**
     * @brief Deleted. Fields must be constructed with owner information available
     *
     */
    SerializableField(const SerializableField& copy) = delete;

    /**
     * @brief Deleted. Fields must be constructed with owner information available
     *
     */
    SerializableField(SerializableField&& copy) = delete;

    /**
     * @brief Assigns the value of this field to the value contained in the copy
     *
     */
    SerializableField& operator=(const SerializableField& copy);

    /**
     * @brief Updates the value of this field from the json data
     *
     * @param json JSON value to deserialize from
     * @return True if successfully deserialized, false on error
     */
    virtual bool deserialize(const Value& json) override;

    /**
     * @brief Serializes the held value of this field to JSON
     *
     * @return Value JSON value of this field
     */
    virtual Value serialize() const override;

    /**
     * @brief Update the value of this field
     *
     */
    void setValue(const T& value);

    /**
     * @brief Calls setValue with the given value
     *
     * @param value Value to set this field to
     * @return SerializableField& Reference to this field
     */
    SerializableField& operator=(const T& value);

    /**
     * @brief Returns the value of this object
     *
     */
    const T& getValue() const;

    /**
     * @brief Implicit conversion to the underlying type
     *
     */
    operator T() const;

private:
    T value;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
SerializableField<T>::SerializableField(const std::string& name, SerializableObject& owner)
: SerializableFieldBase(name, owner) {}

template<typename T>
SerializableField<T>& SerializableField<T>::operator=(const SerializableField& copy) {
    value = copy.value;
    return *this;
}

template<typename T>
bool SerializableField<T>::deserialize(const Value& v) {
    return Serializer<T>::deserialize(value, v);
}

template<typename T>
Value SerializableField<T>::serialize() const {
    return Serializer<T>::serialize(value);
}

template<typename T>
void SerializableField<T>::setValue(const T& v) {
    value = v;
}

template<typename T>
SerializableField<T>& SerializableField<T>::operator=(const T& v) {
    value = v;
    return *this;
}

template<typename T>
const T& SerializableField<T>::getValue() const {
    return value;
}

template<typename T>
SerializableField<T>::operator T() const {
    return value;
}

} // namespace json
} // namespace bl

#endif
