#ifndef BLIB_SERIALIZATION_JSON_SERIALIZABLEOBJECT_HPP
#define BLIB_SERIALIZATION_JSON_SERIALIZABLEOBJECT_HPP

#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace serial
{
namespace json
{
class SerializableFieldBase;

/**
 * @brief Base class for all serializable objects
 *
 */
struct SerializableObjectBase : private util::NonCopyable {
    /**
     * @brief Deserializes the given json into the fields of this object
     *
     * @param value The json data to populate fields with
     * @param object The object to read into
     * @return True on success, false on error
     */
    bool deserialize(const Group& value, void* object) const;

    /**
     * @brief Serializes this object and its fields into the returned json data
     *
     * @param object The object to serialize
     * @return Group json data containing this object and its fields
     */
    Group serialize(const void* object) const;

protected:
    /**
     * @brief Struct to indicate that no fields are optional
     *
     */
    struct StrictMode {};

    /**
     * @brief Struct to indicate that all fields are optional
     *
     */
    struct RelaxedMode {};

    /**
     * @brief Construct a new Serializable Object Base in strict mode
     *
     */
    SerializableObjectBase(StrictMode&&);

    /**
     * @brief Construct a new Serializable Object Base in relaxed mode
     *
     */
    SerializableObjectBase(RelaxedMode&&);

private:
    SerializableObjectBase() = delete;

    const bool strict;
    std::unordered_map<std::string, const SerializableFieldBase*> fields;

    friend class SerializableFieldBase;
};

/**
 * @brief Base class for user defined classes meant to be serialized to and from JSON. Data
 * members of serializable objects should be of type SerializableField. JSON native types,
 * vector, unordered_map, and nested SerializableObject members are supported out of the box.
 * For user defined types as fields they must either extend SerializableObject themselves, or a
 * specialization of Serializer must be provided for the desired type. SerializableObjects that
 * will be nested must provide a copy constructor that registers each contained field.
 *
 * @tparam T The type of object to serialize
 * @ingroup JSON
 */
template<typename T>
class SerializableObject : public SerializableObjectBase {
    template<typename U>
    struct MakeError : std::false_type {};

    static_assert(
        MakeError<T>::value,
        "Type does not have object descriptor. Specialize SerializableObject or Serializer.");
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
