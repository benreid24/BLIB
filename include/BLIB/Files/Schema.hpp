#ifndef BLIB_FILES_SCHEMA_HPP
#define BLIB_FILES_SCHEMA_HPP

#include <BLIB/Files/JSON.hpp>

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
 * @brief Represents a value and type contrained json field value for schema
 * @ingroup JSON
 *
 */
class Value {
public:
    Value() = default;
    Value(const Value& value);
    Value(const Bool& value);
    Value(const Numeric& value);
    Value(const String& value);
    Value(const List& value);
    Value(const Schema& value);

    Value& operator=(const Value& rhs);

    bool validate(const json::Value& value, bool strict) const;

private:
    typedef std::variant<Bool, String, Numeric, List, Schema> TData;

    json::Value::Type type;
    std::shared_ptr<TData> schema;
};

/**
 * @brief Represents a boolean value required for json schema
 * @ingroup JSON
 *
 */
struct Bool {
    static const Bool Any;
};

/**
 * @brief Represents constrained numeric values for json schema
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
 * @brief Represents a constrained set of string values for json schema
 * @ingroup JSON
 *
 */
struct String {
    std::set<std::string> values;

    static const String Any;
};

/**
 * @brief Represents a constrained list for json schema
 * @ingroup JSON
 *
 */
struct List {
    Value itemType;
    unsigned int minSize;
    std::optional<unsigned int> maxSize;

    List(const Value& type, unsigned int minSize = 0);
    List(const Value& type, unsigned int minSize, unsigned int maxSize);
};

} // namespace schema

/**
 * @brief Represents a nestable schema for JSON objects. Equivilent to json::Group
 * @ingroup JSON
 *
 */
class Schema {
public:
    Schema();
    void overrideStrictValidation(bool strict);

    void addRequiredField(const std::string name, const schema::Value& value);
    void addOptionalField(const std::string name, const schema::Value& value);
    void addChoiceField(const std::string name, const schema::Value& value);

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