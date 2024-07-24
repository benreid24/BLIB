#ifndef BLIB_SERIALIZATION_JSON_JSON_HPP
#define BLIB_SERIALIZATION_JSON_JSON_HPP

#include <map>
#include <optional>
#include <ostream>
#include <set>
#include <string>
#include <variant>
#include <vector>

namespace bl
{
namespace serial
{
/// Collection of classes and utilities for interacting with JSON files
namespace json
{
class Value;
class Group;
typedef std::vector<Value> List;

/**
 * @brief Holds source location of any json object
 * @ingroup JSON
 *
 */
struct SourceInfo {
    std::string filename;
    int lineNumber;
};

/**
 * @brief Base class for all json types
 * @ingroup JSON
 *
 */
class Base {
public:
    void setSource(const SourceInfo& source);
    const SourceInfo& source() const;

private:
    SourceInfo info;
};

/**
 * @brief A collection of fields as (std::string, Value) pairs. This is the top level data type
 *        that should be used
 * @ingroup JSON
 *
 */
class Group : public Base {
public:
    /**
     * @brief Returns the list of field names that exist in the Group
     *
     */
    const std::set<std::string>& getFields() const;

    /**
     * @brief Adds or overwrites the given field
     *
     * @param name Name of the field to add
     * @param value The Value of the field
     */
    void addField(const std::string& name, const Value& value);

    /**
     * @brief Tests for the presence of the given field
     *
     * @param name Field to check for
     * @return bool True if the field exists in the Group
     */
    bool hasField(const std::string& name) const;

    /**
     * @brief Returns the field with the given name, if any
     *
     * @param name Name of the field to retrieve
     * @return Value* Pointer to the contained value. nullptr is returned if not found
     */
    Value* getField(const std::string& name);

    /**
     * @brief Returns the field with the given name, if any
     *
     * @param name Name of the field to retrieve
     * @return const Value* Pointer to the contained value. nullptr is returned if not found
     */
    const Value* getField(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a Bool type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return const bool* Returns the contained bool, or nullptr if not found
     */
    const bool* getBool(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a Bool type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return bool* Returns the contained bool, or nullptr if not found
     */
    bool* getBool(const std::string& name);

    /**
     * @brief Shortcut method to retrieve an Integer or Float type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return long The numeric value as an integer
     */
    long getInteger(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve an Integer or Float type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return float The numeric value as a float
     */
    float getFloat(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a String type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return const std::string* Pointer to the contained string, nullptr if not found
     */
    const std::string* getString(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a String type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return std::string* Pointer to the contained string, nullptr if not found
     */
    std::string* getString(const std::string& name);

    /**
     * @brief Shortcut method to retrieve a Group type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return const Group* Pointer to the contained group, nullptr if not found
     */
    const Group* getGroup(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a Group type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return Group* Pointer to the contained group, nullptr if not found
     */
    Group* getGroup(const std::string& name);

    /**
     * @brief Shortcut method to retrieve a List type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return const List* Pointer to the contained list, nullptr if not found
     */
    const List* getList(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a List type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return List* Pointer to the contained list, nullptr if not found
     */
    List* getList(const std::string& name);

    /**
     * @brief Prints the json group to the given stream. This prints valid json that can be
     * parsed
     *
     * @param stream Stream to output to
     * @param indentLevel Current indent level. Used for pretty printing and indenting
     * subgroups
     */
    void print(std::ostream& stream, int indentLevel) const;

    /**
     * @brief Removes all fields from the group
     */
    void clear();

private:
    std::set<std::string> fieldNames;
    std::map<std::string, Value> fields;
};

/**
 * @brief Represents any value type in JSON
 * @ingroup JSON
 *
 */
class Value : public Base {
public:
    /**
     * @brief Represents the currently stored type
     *
     */
    enum struct Type { Bool, String, Integer, Float, Group, List };

    /**
     * @brief Copies from another Value
     *
     */
    Value(const Value& value);

    /**
     * @brief Initializes the Value with either an integer or a boolean value
     *
     * @tparam T Integer type or bool
     * @param intOrBool Integer value or boolean value
     */
    template<typename T, class = std::enable_if<std::is_integral_v<T>>>
    Value(T intOrBool) {
        *this = intOrBool;
    }

    /**
     * @brief Makes a Float Value
     *
     */
    Value(float value);

    /**
     * @brief Makes a String Value
     *
     */
    Value(const std::string& value);

    /**
     * @brief Makes a String Value
     *
     */
    Value(const char* value);

    /**
     * @brief Makes a List Value
     *
     */
    Value(const List& value);

    /**
     * @brief Makes a Group Value
     *
     */
    Value(const Group& value);

    /**
     * @brief Initializes the Value with either an integer or a boolean value
     *
     * @tparam T Integer type or bool
     * @param intOrBool Integer value or boolean value
     * @return A reference to this Value
     */
    template<typename T, class = std::enable_if<std::is_integral_v<T>>>
    Value& operator=(T intOrBool) {
        if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
            type = Type::Bool;
            data = intOrBool;
        }
        else {
            type = Type::Integer;
            data = static_cast<long>(intOrBool);
        }
        return *this;
    }

    Value& operator=(const Value& rhs);
    Value& operator=(float value);
    Value& operator=(const std::string& value);
    Value& operator=(const char* value);
    Value& operator=(const List& value);
    Value& operator=(const Group& value);

    /**
     * @brief Returns the type of the underlying data
     *
     */
    Type getType() const;

    /**
     * @brief Returns the value as a Bool. May return nullptr
     *
     */
    const bool* getAsBool() const;

    /**
     * @brief Returns the value as a Bool. May return nullptr
     *
     */
    bool* getAsBool();

    /**
     * @brief Returns the value as a float, may be nullptr
     *
     */
    const long* getAsInteger() const;

    /**
     * @brief Returns the value as a float, may be nullptr
     *
     */
    long* getAsInteger();

    /**
     * @brief Returns the value as a float, may be nullptr
     *
     */
    const float* getAsFloat() const;

    /**
     * @brief Returns the value as a float, may be nullptr
     *
     */
    float* getAsFloat();

    /**
     * @brief Returns the current numeric value as an integer. Must be either an Integer or Float
     *
     */
    long getNumericAsInteger() const;

    /**
     * @brief Returns the current numeric value as a float. Must be either an Integer or Float
     *
     */
    float getNumericAsFloat() const;

    /**
     * @brief Returns the value as a String, may be nullptr
     *
     */
    const std::string* getAsString() const;

    /**
     * @brief Returns the value as a String, may be nullptr
     *
     */
    std::string* getAsString();

    /**
     * @brief Returns the value as a Group, may be nullptr
     *
     */
    const Group* getAsGroup() const;

    /**
     * @brief Returns the value as a Group, may be nullptr
     *
     */
    Group* getAsGroup();

    /**
     * @brief Returns the value as a List, may be nullptr
     *
     */
    const List* getAsList() const;

    /**
     * @brief Returns the value as a List, may be nullptr
     *
     */
    List* getAsList();

    /**
     * @brief Prints the value to the given stream
     *        Output is valid json but can only be parsed if Group type or
     *        if used by a Group holding this Value
     *
     * @param stream Stream to print to
     * @param indentLevel How much to indent. Used for pretty printing
     */
    void print(std::ostream& stream, int indentLevel) const;

private:
    Type type;
    std::variant<bool, long, std::string, float, Group, List> data;
};

std::ostream& operator<<(std::ostream& stream, const SourceInfo& info);
std::ostream& operator<<(std::ostream& stream, const Value::Type& type);
std::ostream& operator<<(std::ostream& stream, const Group& type);
std::ostream& operator<<(std::ostream& stream, const Value& type);
std::ostream& operator<<(std::ostream& stream, const List& list);

/**
 * @brief Loads json data from the given stream and returns the constructed Group
 *
 * @param stream Stream containing valid json
 * @param result JSON Group to read into
 * @return True on success, false on error
 */
bool loadFromStream(std::istream& stream, json::Group& result);

/**
 * @brief Loads json data from the given file
 *
 * @param file Filename to load from
 * @param result Group to read into
 * @return True on success, false on error
 */
bool loadFromFile(const std::string& file, json::Group& result);

/**
 * @brief Saves a json::Group to the given file
 *
 * @param file Filename to save to
 * @param data The top level json data to save
 * @return True on success, false on error
 */
bool saveToFile(const std::string& file, const json::Group& data);

/**
 * @brief Writes a json::Group to the given stream
 *
 * @param stream The stream to write to
 * @param data The top level json data to write
 * @return True on success, false on error
 */
bool saveToStream(std::ostream& stream, const json::Group& data);

} // namespace json
} // namespace serial
} // namespace bl

#endif
