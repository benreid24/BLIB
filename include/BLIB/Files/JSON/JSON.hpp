#ifndef BLIB_FILES_JSON_JSON_HPP
#define BLIB_FILES_JSON_JSON_HPP

#include <map>
#include <optional>
#include <ostream>
#include <set>
#include <string>
#include <variant>
#include <vector>

/**
 * @defgroup JSON
 * @brief Collection of classes for interacting with JSON data
 *
 */

namespace bl
{
namespace json
{
class Value;
class Group;
typedef std::vector<Value> List;

typedef std::optional<float> Numeric;
typedef std::optional<std::string> String;
typedef std::optional<bool> Bool;
typedef std::optional<Group> RGroup;
typedef std::optional<List> RList;
typedef std::optional<Value> RValue;

/**
 * @brief Holds source location of any json object
 * @ingroup JSON
 * @ingroup Files
 *
 */
struct SourceInfo {
    std::string filename;
    int lineNumber;
};

/**
 * @brief Base class for all json types
 * @ingroup JSON
 * @ingroup Files
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
     * @return RValue std::optional containing the Value of the field, or null if not present
     */
    RValue getField(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a Bool type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return Bool std::optional with the Value, or null if not present
     */
    Bool getBool(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a Numeric type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return Numeric std::optional with the Value, or null if not present
     */
    Numeric getNumeric(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a String type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return String std::optional with the Value, or null if not present
     */
    String getString(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a Group type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return RGroup std::optional with the Value, or null if not present
     */
    RGroup getGroup(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a List type field
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return RList std::optional with the Value, or null if not present
     */
    RList getList(const std::string& name) const;

    /**
     * @brief Prints the json group to the given stream. This prints valid json that can be
     * parsed
     *
     * @param stream Stream to output to
     * @param indentLevel Current indent level. Used for pretty printing and indenting
     * subgroups
     */
    void print(std::ostream& stream, int indentLevel) const;

private:
    std::set<std::string> fieldNames;
    std::map<std::string, Value> fields;
};

/**
 * @brief Represents any value type in JSON
 * @ingroup JSON
 * @ingroup Files
 *
 */
class Value : public Base {
public:
    /**
     * @brief Represents the currently stored type
     *
     */
    enum Type { TBool, TString, TNumeric, TGroup, TList };

    /**
     * @brief Copies from another Value
     *
     */
    Value(const Value& value);

    /**
     * @brief Makes a Bool Value
     *
     */
    explicit Value(bool value);

    /**
     * @brief Makes a Numeric Value
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

    Value& operator=(const Value& rhs);
    Value& operator=(bool value);
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
     * @brief Returns the value as a Bool
     *
     * @return Bool std::optional with the value, or null if wrong type
     */
    Bool getAsBool() const;

    /**
     * @brief Returns the value as a Numeric
     *
     * @return Numeric std::optional with the value, or null if wrong type
     */
    Numeric getAsNumeric() const;

    /**
     * @brief Returns the value as a String
     *
     * @return String std::optional with the value, or null if wrong type
     */
    String getAsString() const;

    /**
     * @brief Returns the value as a Group
     *
     * @return RGroup std::optional with the value, or null if wrong type
     */
    RGroup getAsGroup() const;

    /**
     * @brief Returns the value as a List
     *
     * @return RList std::optional with the value, or null if wrong type
     */
    RList getAsList() const;

    /**
     * @brief Shortcut method to retrieve a Bool type field. Only works on Group types
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return Bool std::optional with the Value, or null if not present
     */
    Bool getBool(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a Numeric type field. Only works on Group types
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return Numeric std::optional with the Value, or null if not present
     */
    Numeric getNumeric(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a String type field. Only works on Group types
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return String std::optional with the Value, or null if not present
     */
    String getString(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a Group type field. Only works on Group types
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return RGroup std::optional with the Value, or null if not present
     */
    RGroup getGroup(const std::string& name) const;

    /**
     * @brief Shortcut method to retrieve a List type field. Only works on Group types
     *        The field name can be nested, ex "subgroup/fieldname"
     *
     * @param name Name of the field, can contain multiple levels of groups
     * @return RList std::optional with the Value, or null if not present
     */
    RList getList(const std::string& name) const;

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
    std::variant<bool, std::string, float, Group, List> data;
};

std::ostream& operator<<(std::ostream& stream, const SourceInfo& info);
std::ostream& operator<<(std::ostream& stream, const Value::Type& type);
std::ostream& operator<<(std::ostream& stream, const Group& type);
std::ostream& operator<<(std::ostream& stream, const Value& type);
std::ostream& operator<<(std::ostream& stream, const List& list);

} // namespace json

/**
 * @brief Top level interface for loading and saving json data
 * @ingroup JSON
 * @ingroup Files
 *
 */
struct JSON {
    /**
     * @brief Loads json data from the given stream and returns the constructed Group
     *
     * @param stream Stream containing valid json
     * @return json::Group Group containing loaded data. Empty on error
     */
    static json::Group loadFromStream(std::istream& stream);

    /**
     * @brief Loads json data from the given file
     *
     * @param file Filename to load from
     * @return json::Group Group containing loaded data. Empty on error
     */
    static json::Group loadFromFile(const std::string& file);

    /**
     * @brief Loads json from the supplied string
     *
     * @param data String containing valid json
     * @return json::Group Group containing loaded data. Empty on error
     */
    static json::Group loadFromString(const std::string& data);

    /**
     * @brief Saves a json::Group to the given file
     *
     * @param file Filename to save to
     * @param data The top level json data to save
     */
    static void saveToFile(const std::string& file, const json::Group& data);

    /**
     * @brief Writes a json::Group to the given stream
     *
     * @param stream The stream to write to
     * @param data The top level json data to write
     */
    static void saveToStream(std::ostream& stream, const json::Group& data);
};

} // namespace bl

#endif