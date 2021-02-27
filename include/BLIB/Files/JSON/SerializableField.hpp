#ifndef BLIB_FILES_JSON_SERIALIZABLEFIELD_HPP
#define BLIB_FILES_JSON_SERIALIZABLEFIELD_HPP

#include <BLIB/Files/JSON/JSON.hpp>
#include <BLIB/Files/JSON/SerializableObject.hpp>
#include <BLIB/Files/JSON/Serializer.hpp>

namespace bl
{
namespace json
{
class SerializableFieldBase {
public:
    virtual bool deserialize(const Value& json) = 0;

    virtual Value serialize() const = 0;

    const std::string& getName() const;

protected:
    SerializableFieldBase(const std::string& name, SerializableObject& owner);

private:
    const std::string name;
};

template<typename T>
class SerializableField : public SerializableFieldBase {
public:
    SerializableField(const std::string& name, SerializableObject& owner);

    virtual bool deserialize(const Value& json) override;

    virtual Value serialize() const override;

    void setValue(const T& value);

    const T& getValue() const;

private:
    T value;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
SerializableField<T>::SerializableField(const std::string& name, SerializableObject& owner)
: SerializableFieldBase(name, owner) {}

template<typename T>
bool SerializableField<T>::deserialize(const Value& v) {
    return Serializer::deserialize(value, v);
}

template<typename T>
Value SerializableField<T>::serialize() const {
    return Serializer::serialize(value);
}

template<typename T>
void SerializableField<T>::setValue(const T& v) {
    value = v;
}

template<typename T>
const T& SerializableField<T>::getValue() const {
    return value;
}

} // namespace json
} // namespace bl

#endif
