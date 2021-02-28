#ifndef BLIB_FILES_JSON_SCHEMA_HPP
#define BLIB_FILES_JSON_SCHEMA_HPP

#include <BLIB/Files/JSON/JSON.hpp>

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <variant>

namespace bl
{
namespace json
{
class Schema;

namespace schema
{
class Bool;
class Numeric;
class String;
class List;

/**
 * @brief Represents a constraint on the type and value of a json::Value
 * @ingroup JSON
 *
 */
class Value {
public:
    /**
     * @brief Empty Value. Not valid for use in Schema
     *
     */
    Value() = default;

    /**
     * @brief Copies the Value
     *
     */
    Value(const Value& value);

    /**
     * @brief The Value must have a Bool type
     *
     */
    Value(const Bool& value);

    /**
     * @brief The Value must have a Numeric type
     *
     */
    Value(const Numeric& value);

    /**
     * @brief The Value must have a String type
     *
     */
    Value(const String& value);

    /**
     * @brief The Value must have a List type
     *
     */
    Value(const List& value);

    /**
     * @brief The Value must have a Group type
     *
     */
    Value(const Schema& value);

    Value& operator=(const Value& rhs);

    /**
     * @brief Validates a json::Value with the expected schema value
     *
     * @param value The value to check
     * @param strict For group types. strict checking fails if extra fields present
     * @return bool True if the value falls within the expectation
     */
    bool validate(const json::Value& value, bool strict) const;

private:
    typedef std::variant<Bool, String, Numeric, List, Schema> TData;

    json::Value::Type type;
    std::shared_ptr<TData> schema;
};

/**
 * @brief Represents a boolean value required for json::Value
 * @ingroup JSON
 *
 */
struct Bool {
    static const Bool Any;
};

/**
 * @brief Represents a constrained numeric value for a json::Value. Can optionally supply a
 *        minimum and/or maximum value
 * @ingroup JSON
 *
 */
struct Numeric {
    std::optional<float> min;
    std::optional<float> max;

    static const Numeric Any;
    static const Numeric Positive;
    static const Numeric Negative;
};

/**
 * @brief Represents a constrained string value for json::Value. Can optionally supply a set of
 *        strings that the value must be
 * @ingroup JSON
 *
 */
struct String {
    std::set<std::string> values;
    String(const std::set<std::string>& values)
    : values(values) {}

    static const String Any;
};

/**
 * @brief Represents a constrained list for json::Value. Can supply optional limits on min and
 *        max size, as well as supply a Value to validate the contained items
 * @ingroup JSON
 *
 */
struct List {
    Value itemType;
    unsigned int minSize;
    std::optional<unsigned int> maxSize;

    /**
     * @brief Creates a new List expectation of a list with the given type and minimum size,
     *        unbouned maximum size
     *
     * @param type The Value expectation for list elements
     * @param minSize The minimum list size
     */
    List(const Value& type, unsigned int minSize = 0);

    /**
     * @brief Creates a new List expectation of a list with the given type, minimum size,
     *        and maximum size
     *
     * @param type The Value expectation for list elements
     * @param minSize The minimum list size
     * @param maxSize The maximum list size
     */
    List(const Value& type, unsigned int minSize, unsigned int maxSize);

    /**
     * @brief Validates the given json::List against the expectations set, including type and
     * size
     *
     * @param source json source info for error printing
     * @param list The list to validate
     * @param strict For group types. strict checking fails if extra fields present
     * @return True if the suppliedl list meets expectations
     */
    bool validate(const SourceInfo& source, const json::List& list, bool strict) const;
};

} // namespace schema

/**
 * @brief Represents a nestable schema for JSON objects. Equivilent to json::Group
 *        Schema can be used to define a data format for json, allowing for fields to be
 * expected and constraining their types and values. Creating a Schema for loading data can
 *        elimate the need for verbose error checking/handling in the code that reads the
 *        json data
 * @ingroup JSON
 *
 */
class Schema {
public:
    /**
     * @brief Represents an empty json::Group
     *
     */
    Schema();

    /**
     * @brief Allows this group to override the strict validation setting.
     *        This setting is useful for when you want strict validation on data but
     *        want to exclude a nested group from strict validation
     *
     * @param strict
     */
    void overrideStrictValidation(bool strict);

    /**
     * @brief Adds a required field to the data expectation. Validation fails if the field is
     *       not present
     *
     * @param name The name of the field to expect
     * @param value The Value to validate the field with
     */
    void addRequiredField(const std::string name, const schema::Value& value);

    /**
     * @brief Adds an optional field that may be present, but does not fail validation if
     *        missing. Type check still required
     *
     * @param name Name of the field that may be present
     * @param value The Value to validate the field with
     */
    void addOptionalField(const std::string name, const schema::Value& value);

    /**
     * @brief Adds an option for an enum type json::Group. Allows a group to expect exactly one
     *        field from a list of possible fields
     *
     * @param name The name of one of the possible fields
     * @param value The Value to validate the field of that name
     */
    void addChoiceField(const std::string name, const schema::Value& value);

    /**
     * @brief Validates the given json::Group against the set expectations
     *
     * @param root The json::Group to validate
     * @param strict Strict checking will fail if fields are present that were not expected
     * @return bool True if the Group meets the set data expectations, false otherwise
     */
    bool validate(const Group& root, bool strict) const;

private:
    bool overrideStrict;
    bool isStrict;
    std::map<std::string, schema::Value> requiredFields;
    std::map<std::string, schema::Value> optionalFields;
    std::map<std::string, schema::Value> choiceFields;
};

} // namespace json
} // namespace bl

#endif