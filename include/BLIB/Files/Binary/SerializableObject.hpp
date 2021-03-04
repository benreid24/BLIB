#ifndef BLIB_FILES_BINARY_SERIALIZABLEOBJECT_HPP
#define BLIB_FILES_BINARY_SERIALIZABLEOBJECT_HPP

#include <BLIB/Files/Binary/BinaryFile.hpp>

#include <vector>

namespace bl
{
namespace bf
{
class SerializableFieldBase;

/**
 * @brief Base class for user defined objects that can be serialized in binary files. Data fields to
 *        be serialized should be stored as SerializableField objects
 *
 * @ingroup BinaryFiles
 */
class SerializableObject {
public:
    /**
     * @brief Default constructor
     *
     */
    SerializableObject() = default;

    /**
     * @brief Does not copy over any fields. Care must be taken to properly construct member fields
     *        in user defined classes
     *
     */
    SerializableObject(const SerializableObject& copy);

    /**
     * @brief Does not copy over any fields. Care must be taken to properly construct member fields
     *        in user defined classes
     *
     */
    SerializableObject(SerializableObject&& copy);

    /**
     * @brief Does not copy over any fields. Care must be taken to properly construct member fields
     *        in user defined classes
     *
     */
    SerializableObject& operator=(const SerializableObject& copy);

    /**
     * @brief Serializes this object and its contained fields to the given file
     *
     * @param output The file to write to
     * @return True on success, false if data could not be written
     */
    bool serialize(BinaryFile& output) const;

    /**
     * @brief Loads the fields of this object from the given file
     *
     * @param input The file to read from
     * @return True on success, false if not all fields could be read
     */
    bool deserialize(BinaryFile& input);

private:
    std::vector<SerializableFieldBase*> fields;

    void addField(SerializableFieldBase* field);

    friend class SerializableFieldBase;
};

} // namespace bf
} // namespace bl

#endif
