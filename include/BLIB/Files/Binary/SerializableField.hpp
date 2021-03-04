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

    virtual std::uint32_t size() const = 0;

    virtual std::uint16_t id() const = 0;

protected:
    SerializableFieldBase(SerializableObject& owner, std::uint16_t id);
};

/**
 * @brief Template class that represents a serializable field for BinaryFile use. Strings and
 *        integral types are supported out of the box, as well as SerializableObjects. User
 *        defined types must either implement SerializableObject or a custom version of Serializer
 *
 * @tparam Id The id of this field. Must be unique within a SerializableObject
 * @tparam T The type of the field to be serialized
 * @ingroup BinaryFiles
 */
template<std::uint16_t Id, typename T>
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
     * @brief Returns the id of this field
     *
     * @return std::uint16_t The id of this field
     */
    virtual std::uint16_t id() const override;

    /**
     * @brief Returns the serialized size of this field
     *
     * @return std::uint32_t The number of bytes this field serializes to
     */
    virtual std::uint32_t size() const override;

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

template<std::uint16_t Id, typename T>
SerializableField<Id, T>::SerializableField(SerializableObject& owner)
: SerializableFieldBase(owner, Id) {}

template<std::uint16_t Id, typename T>
bool SerializableField<Id, T>::serialize(BinaryFile& output) const {
    return Serializer<T>::serialize(output, value);
}

template<std::uint16_t Id, typename T>
bool SerializableField<Id, T>::deserialize(BinaryFile& input) {
    return Serializer<T>::deserialize(input, value);
}

template<std::uint16_t Id, typename T>
std::uint16_t SerializableField<Id, T>::id() const {
    return Id;
}

template<std::uint16_t Id, typename T>
std::uint32_t SerializableField<Id, T>::size() const {
    return Serializer<T>::size(value);
}

template<std::uint16_t Id, typename T>
const T& SerializableField<Id, T>::getValue() const {
    return value;
}

template<std::uint16_t Id, typename T>
void SerializableField<Id, T>::setValue(const T& v) {
    value = v;
}

template<std::uint16_t Id, typename T>
SerializableField<Id, T>& SerializableField<Id, T>::operator=(const T& v) {
    value = v;
    return *this;
}

template<std::uint16_t Id, typename T>
SerializableField<Id, T>::operator T() const {
    return value;
}

} // namespace bf
} // namespace bl

#endif
