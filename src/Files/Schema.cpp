#include <BLIB/Files/Schema.hpp>

#include <BLIB/Logging.hpp>
#include <algorithm>

#define SCHEMA_ERROR(source)                                       \
    BL_LOG_ERROR << "VALIDATION FAILED: File '" << source.filename \
                 << "':" << source.lineNumber << ": "

namespace bl
{
namespace json
{
namespace schema
{
const Bool Bool::Any;
const Numeric Numeric::Any      = {{}, {}};
const Numeric Numeric::Positive = {0, {}};
const Numeric Numeric::Negative = {{}, 0};
const String String::Any({});

List::List(const Value& type, unsigned int minSize)
: itemType(type)
, minSize(minSize) {}

List::List(const Value& type, unsigned int minSize, unsigned int maxSize)
: itemType(type)
, minSize(minSize)
, maxSize(maxSize) {}

bool List::validate(const SourceInfo& source, const json::List& list, bool strict) const {
    bool valid = true;
    if (list.size() < minSize) {
        SCHEMA_ERROR(source) << "List is too small, min size is " << minSize;
        valid = false;
    }
    if (maxSize) {
        if (list.size() > maxSize.value()) {
            SCHEMA_ERROR(source) << "List size is too large, max size is " << maxSize.value();
            valid = false;
        }
    }
    for (const json::Value& val : list) {
        if (!itemType.validate(val, strict)) valid = false;
    }
    return valid;
}

Value::Value(const Value& value)
: type(value.type)
, schema(value.schema) {}

Value::Value(const Bool& value)
: type(json::Value::TBool)
, schema(new TData(value)) {}

Value::Value(const Numeric& value)
: type(json::Value::TNumeric)
, schema(new TData(value)) {}

Value::Value(const String& value)
: type(json::Value::TString)
, schema(new TData(value)) {}

Value::Value(const List& value)
: type(json::Value::TList)
, schema(new TData(value)) {}

Value::Value(const Schema& value)
: type(json::Value::TGroup)
, schema(new TData(value)) {}

bool Value::validate(const json::Value& value, bool strict) const {
    if (!schema) {
        BL_LOG_ERROR << "Invalid schema::Value\n";
        return false;
    }

    if (value.getType() != type) {
        SCHEMA_ERROR(value.source())
            << "Type mismatch. Expected " << type << " got " << value.getType();
        return false;
    }

    switch (type) {
    case json::Value::TBool:
        return true;
    case json::Value::TNumeric:
        if (std::get<Numeric>(*schema).min) {
            if (value.getAsNumeric().value() < std::get<Numeric>(*schema).min.value()) {
                SCHEMA_ERROR(value.source()) << "Value is too small, minimum is "
                                             << std::get<Numeric>(*schema).min.value();
                return false;
            }
        }
        if (std::get<Numeric>(*schema).max) {
            if (value.getAsNumeric().value() > std::get<Numeric>(*schema).max.value()) {
                SCHEMA_ERROR(value.source()) << "Value is too large, maximum is "
                                             << std::get<Numeric>(*schema).max.value();
                return false;
            }
        }
        return true;
    case json::Value::TString:
        if (!std::get<String>(*schema).values.empty()) {
            const auto& set = std::get<String>(*schema).values;
            if (std::find(set.begin(), set.end(), value.getAsString().value()) == set.end()) {
                std::stringstream ss;
                ss << "Invalid value '" << value.getAsString().value() << " must be in [ ";
                for (const auto& s : set) { ss << "'" << s << "' "; }
                ss << "]";
                SCHEMA_ERROR(value.source()) << ss.str();
                return false;
            }
        }
        return true;
    case json::Value::TGroup:
        return std::get<Schema>(*schema).validate(value.getAsGroup().value(), strict);
    case json::Value::TList:
        return std::get<List>(*schema).validate(
            value.source(), value.getAsList().value(), strict);
    default:
        SCHEMA_ERROR(value.source()) << "Invalid schema type " << type;
        return false;
    }
}
} // namespace schema

Schema::Schema()
: overrideStrict(false)
, isStrict(false) {}

void Schema::overrideStrictValidation(bool strict) {
    overrideStrict = true;
    isStrict       = strict;
}

void Schema::addRequiredField(const std::string name, const schema::Value& value) {
    requiredFields.insert(std::make_pair(name, value));
}

void Schema::addOptionalField(const std::string name, const schema::Value& value) {
    optionalFields.insert(std::make_pair(name, value));
}

void Schema::addChoiceField(const std::string name, const schema::Value& value) {
    choiceFields.insert(std::make_pair(name, value));
}

bool Schema::validate(const Group& root, bool strict) const {
    const bool beStrict = overrideStrict ? isStrict : strict;
    bool valid          = true;

    std::vector<std::string> reqFlds;
    bool choiceMade = false;

    reqFlds.reserve(requiredFields.size());
    for (const auto& f : requiredFields) reqFlds.push_back(f.first);

    for (const std::string& field : root.getFields()) {
        const Value val = root.getField(field).value();
        auto i          = std::find(reqFlds.begin(), reqFlds.end(), field);
        if (i != reqFlds.end()) {
            reqFlds.erase(i);
            if (!requiredFields.find(field)->second.validate(val, strict)) valid = false;
            continue;
        }
        auto j = optionalFields.find(field);
        if (j != optionalFields.end()) {
            if (!j->second.validate(val, strict)) valid = false;
            continue;
        }
        j = choiceFields.find(field);
        if (j != choiceFields.end()) {
            if (choiceMade) {
                std::stringstream ss;
                ss << "Extra field '" << field << "'. Must only have one of [ ";
                for (const auto& f : choiceFields) { ss << "'" << f.first << "' "; }
                ss << "]";
                SCHEMA_ERROR(val.source()) << ss.str();
                valid = false;
            }
            choiceMade = true;
            continue;
        }
        if (beStrict) {
            SCHEMA_ERROR(val.source()) << "Extra field '" << field << "'";
            valid = false;
        }
    }

    if (!reqFlds.empty()) {
        std::stringstream ss;
        ss << "Missing required fields [ ";
        for (const auto f : reqFlds) ss << "'" << f << "' ";
        ss << "]";
        SCHEMA_ERROR(root.source()) << ss.str();
        valid = false;
    }

    return valid;
}

} // namespace json

} // namespace bl
