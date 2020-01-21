#ifndef BLIB_FILES_JSONTYPES_HPP
#define BLIB_FILES_JSONTYPES_HPP

#include <optional>
#include <variant>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <ostream>

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
std::ostream& operator<<(std::ostream& stream, const SourceInfo& info);

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
 * @brief A collection of fields as (std::string, Value) pairs
 * @ingroup JSON
 * 
 */
class Group : public Base {
public:
    const std::set<std::string>& getFields() const;
    void addField(const std::string& name, const Value& value);
    bool hasField(const std::string& name) const;
    RValue getField(const std::string& name) const;

    Bool getBool(const std::string& name) const;
    Numeric getNumeric(const std::string& name) const;
    String getString(const std::string& name) const;
    RGroup getGroup(const std::string& name) const;
    RList getList(const std::string& name) const;

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
    enum Type {
        TBool,
        TString,
        TNumeric,
        TGroup,
        TList
    };

    Value(const Value& value);
    Value(bool value);
    Value(float value);
    Value(const std::string& value);
    Value(const List& value);
    Value(const Group& value);

    Value& operator=(const Value& rhs);
    Value& operator=(bool value);
    Value& operator=(float value);
    Value& operator=(const std::string& value);
    Value& operator=(const List& value);
    Value& operator=(const Group& value);

    Type getType() const;
    Bool getAsBool() const;
    Numeric getAsNumeric() const;
    String getAsString() const;
    RGroup getAsGroup() const;
    RList getAsList() const;

    // Shortcuts for Group type Values
    Bool getBool(const std::string& name) const;
    Numeric getNumeric(const std::string& name) const;
    String getString(const std::string& name) const;
    RGroup getGroup(const std::string& name) const;
    RList getList(const std::string& name) const;

    void print(std::ostream& stream, int indentLevel) const;

private:
    Type type;
    std::variant<bool, std::string, float, Group, List> data;
};

std::ostream& operator<<(std::ostream& stream, const Value::Type& type);

std::ostream& operator<<(std::ostream& stream, const List& list);

}

/**
 * @brief Public interface for loading and saving json data
 * @ingroup JSON
 * @ingroup Files
 * 
 */
struct JSON {
    static json::Group loadFile(const std::string& file);
    static void saveFile(const std::string& file, const json::Group& data);
};

}

#endif