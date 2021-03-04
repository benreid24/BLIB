#ifndef BLIB_FILES_BINARY_SERIALIZABLEFIELD_HPP
#define BLIB_FILES_BINARY_SERIALIZABLEFIELD_HPP

#include <BLIB/Files/Binary/BinaryFile.hpp>
#include <BLIB/Files/Binary/Serializer.hpp>

namespace bl
{
namespace bf
{
class SerializableObject;

/**
 * @brief Base class for binary serializable fields. Do not use
 *
 * @ingroup BinaryFiles
 *
 */
class SerializableFieldBase {
public:
    virtual bool serialize(BinaryFile& output) const = 0;

    virtual bool deserialize(BinaryFile& input) = 0;

protected:
    SerializableFieldBase(SerializableObject& owner);
};

/**
 * @brief Template class that represents a serializable field for BinaryFile use. Strings and
 *        integral types are supported out of the box, as well as SerializableObjects. User
 *        defined types must either implement SerializableObject or a custom version of Serializer
 *
 * @tparam T The type of the field to be serialized
 * @ingroup BinaryFiles
 */
template<typename T>
class SerializableField : public SerializableFieldBase {
public:
    /**
     * @brief Creates the new field and links it to its owning object
     *
     * @param owner The owner of this field
     */
    SerializableField(SerializableObject& owner);

    /**
     * @brief Writes the value of this field to the given file
     *
     * @param output The file to write to
     * @return True on success, false if the file is in a bad state
     */
    virtual bool serialize(BinaryFile& output) const override;

    /**
     * @brief Populates this field value with data read from the given file
     *
     * @param input File to read from
     * @return True if read successfully, false if file is in bad state
     */
    virtual bool deserialize(BinaryFile& input) override;

    /**
     * @brief Explicit accessor for the field value
     *
     */
    const T& getValue() const;

    /**
     * @brief Explicit setter for the field value
     *
     * @param v The new value to set to
     */
    void setValue(const T& v);

    /**
     * @brief Assigns a new value to this field
     *
     * @param v The value to assign to
     * @return SerializableField& A reference to this field
     */
    SerializableField& operator=(const T& v);

    /**
     * @brief Implicit conversion operator for convenience
     *
     */
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
