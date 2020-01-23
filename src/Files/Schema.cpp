#include <BLIB/Files/Schema.hpp>

#include <iostream>

namespace bl
{
namespace json
{
namespace schema
{
namespace
{
std::ostream& error(const SourceInfo& source) {
    std::cerr << "VALIDATION ERROR: File '" << source.filename << "':" << source.lineNumber << ": ";
    return std::cerr;
}

} // namespace
Value::Value(const Value& value)
: type(value.type)
, schema(value.schema) {
}

Value::Value(const Bool& value)
: type(json::Value::TBool)
, schema(new TData(value)) {
}

Value::Value(const Numeric& value)
: type(json::Value::TNumeric)
, schema(new TData(value)) {
}

Value::Value(const String& value)
: type(json::Value::TString)
, schema(new TData(value)) {
}

Value::Value(const List& value)
: type(json::Value::TList)
, schema(new TData(value)) {
}

Value::Value(const Schema& value)
: type(json::Value::TGroup)
, schema(new TData(value)) {
}

bool Value::validate(const json::Value& value, bool strict) const {
    if (value.getType() != type) {
        error(value.source()) << "Type mismatch. Expected " << type << " got " << value.getType()
                              << std::endl;
        return false;
    }

    switch (type) {
    case json::Value::TBool:
        return true;
    case json::Value::TNumeric:
        // TODO - bounds check
        return true;
    case json::Value::TString:
        // TODO - set check
        return true;
    case json::Value::TGroup:
        return std::get<Schema>(*schema).validate(value.getAsGroup().value(), strict);
    case json::Value::TList:
        // TODO - validate length and type
        return true;
    default:
        error(value.source()) << "Invalid schema type " << type << std::endl;
        return false;
    }
}
} // namespace schema

Schema::Schema()
: overrideStrict(false)
, isStrict(false) {
}

void Schema::overrideStrictValidation(bool strict) {
    overrideStrict = true;
    isStrict       = strict;
}

void Schema::addRequiredField(const std::string name, const schema::Value& value) {
    requiredFields.push_back({name, value});
}

void Schema::addOptionalField(const std::string name, const schema::Value& value) {
    optionalFields.push_back({name, value});
}

void Schema::addChoiceField(const std::string name, const schema::Value& value) {
    choiceFields.push_back({name, value});
}

bool Schema::validate(const Group& root, bool strict) const {
    const bool beStrict = overrideStrict ? isStrict : strict;

    std::vector<std::string> reqFlds;
    std::vector<std::string> optFlds;
    std::vector<std::string> chcFlds;
    // TODO - copy
}

} // namespace json

} // namespace bl
