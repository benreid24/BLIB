#ifndef BLIB_FILES_BINARY_SERIALIZABLEFIELD_HPP
#define BLIB_FILES_BINARY_SERIALIZABLEFIELD_HPP

#include <BLIB/Files/Binary/BinaryFile.hpp>
#include <BLIB/Files/Binary/Serializer.hpp>

namespace bl
{
namespace bf
{
class SerializableObject;

class SerializableFieldBase {
public:
    virtual bool serialize(BinaryFile& output) const = 0;

    virtual bool deserialize(BinaryFile& input) = 0;

protected:
    SerializableFieldBase(SerializableObject& owner);
};

template<typename T>
class SerializableField : public SerializableFieldBase {
public:
    SerializableField(SerializableObject& owner);

    virtual bool serialize(BinaryFile& output) const override;

    virtual bool deserialize(BinaryFile& input) override;

    const T& getValue() const;

    void setValue(const T& v);

    SerializableField& operator=(const T& v);

    operator T() const;

    SerializableField(const SerializableField&) = delete;

private:
    T value;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
SerializableField<T>::SerializableField(SerializableObject& owner)
: SerializableFieldBase(owner) {}

template<typename T>
bool SerializableField<T>::serialize(BinaryFile& output) const {
    return Serializer<T>::serialize(output, value);
}

template<typename T>
bool SerializableField<T>::deserialize(BinaryFile& input) {
    return Serializer<T>::deserialize(input, value);
}

template<typename T>
const T& SerializableField<T>::getValue() const {
    return value;
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
SerializableField<T>::operator T() const {
    return value;
}

} // namespace bf
} // namespace bl

#endif
