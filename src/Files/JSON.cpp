#include <BLIB/Files/JSON.hpp>

namespace bl
{
namespace json
{
namespace
{
RValue getNestedValue(const Group& group, const std::string& name) {
    RGroup nestedGroup = group;
    std::string path = name;

    while (!path.empty()) {
        auto n = path.find_first_of('/');
        if (n != std::string::npos) {
            const std::string subname = path.substr(n);
            RValue val = nestedGroup.value().getField(subname);
            if (!val || !val.value().getAsGroup())
                return std::nullopt;
            nestedGroup = val.value().getAsGroup();
            path.erase(0, n+1);
        }
        else
            return nestedGroup.value().getField(path);
    }

    return std::nullopt;
}
}

void Base::setSource(const SourceInfo& source) {
    info = source;
}

const SourceInfo& Base::source() const {
    return info;
}

const std::set<std::string>& Group::getFields() const {
    return fieldNames;
}

void Group::addField(const std::string& name, const Value& value) {
    fieldNames.insert(name);
    fields[name] = value;
}

bool Group::hasField(const std::string& name) const {
    return fields.find(name) != fields.end();
}

RValue Group::getField(const std::string& name) const {
    auto i = fields.find(name);
    if (i != fields.end())
        return i->second;
    return std::nullopt;
}

Bool Group::getBool(const std::string& name) const {
    RValue val = getNestedValue(*this, name);
    if (val)
        return val.value().getAsBool();
    return std::nullopt;
}

Numeric Group::getNumeric(const std::string& name) const {
    RValue val = getNestedValue(*this, name);
    if (val)
        return val.value().getAsNumeric();
    return std::nullopt;
}

String Group::getString(const std::string& name) const {
    RValue val = getNestedValue(*this, name);
    if (val)
        return val.value().getAsString();
    return std::nullopt;
}

RGroup Group::getGroup(const std::string& name) const {
    RValue val = getNestedValue(*this, name);
    if (val)
        return val.value().getAsGroup();
    return std::nullopt;
}

RList Group::getList(const std::string& name) const {
    RValue val = getNestedValue(*this, name);
    if (val)
        return val.value().getAsList();
    return std::nullopt;
}

Value::Value(const Value& value) {
    *this = value;
}

Value::Value(bool value) {
    *this = value;
}

Value::Value(float value) {
    *this = value;
}

Value::Value(const std::string& value) {
    *this = value;
}

Value::Value(const Group& value) {
    *this = value;
}

Value::Value(const List& value) {
    *this = value;
}

Value& Value::operator=(const Value& value) {
    type = value.type;
    data = value.data;
    return *this;
}

Value& Value::operator=(bool value) {
    type = TBool;
    data = value;
    return *this;
}

Value& Value::operator=(float value) {
    type = TNumeric;
    data = value;
    return *this;
}

Value& Value::operator=(const std::string& value) {
    type = TString;
    data = value;
    return *this;
}

Value& Value::operator=(const List& value) {
    type = TList;
    data = value;
    return *this;
}

Value& Value::operator=(const Group& value) {
    type = TGroup;
    data = value;
    return *this;
}

Value::Type Value::getType() const {
    return type;
}

Bool Value::getAsBool() const {
    if (type == TBool)
        return std::get<bool>(data);
    return std::nullopt;
}

Numeric Value::getAsNumeric() const {
    if (type == TNumeric)
        return std::get<float>(data);
    return std::nullopt;
}

String Value::getAsString() const {
    if (type == TString)
        return std::get<std::string>(data);
    return std::nullopt;
}

RGroup Value::getAsGroup() const {
    if (type == TGroup)
        return std::get<Group>(data);
    return std::nullopt;
}

RList Value::getAsList() const {
    if (type == TList)
        return std::get<List>(data);
    return std::nullopt;
}

Bool Value::getBool(const std::string& name) const {
    if (type == TGroup)
        return getAsGroup().value().getBool(name);
    return std::nullopt;
}

Numeric Value::getNumeric(const std::string& name) const {
    if (type == TGroup)
        return getAsGroup().value().getNumeric(name);
    return std::nullopt;
}

String Value::getString(const std::string& name) const {
    if (type == TGroup)
        return getAsGroup().value().getString(name);
    return std::nullopt;
}

RGroup Value::getGroup(const std::string& name) const {
    if (type == TGroup)
        return getAsGroup().value().getGroup(name);
    return std::nullopt;
}

RList Value::getList(const std::string& name) const {
    if (type == TGroup)
        return getAsGroup().value().getList(name);
    return std::nullopt;
}

std::ostream& operator<<(std::ostream& stream, const SourceInfo& info) {
    stream << info.filename << ":" << info.lineNumber;
    return stream;
}

void Group::print(std::ostream& stream, int ilvl) const {
    stream << "{";
    if (!fields.empty())
        stream << "\n";
    for (auto i = fields.begin(); i!=fields.end(); ) {
        stream << std::string(ilvl, ' ');
        i->second.print(stream, ilvl+4);
        if (++i != fields.end())
            stream << ",\n";
    }
    stream << std::string(ilvl-4, ' ') << "}";
}

void Value::print(std::ostream& stream, int ilvl) const {
    switch (getType())
    {
    case TBool:
        stream << getAsBool().value() ? "true" : "false";
        break;
    case TNumeric:
        stream << getAsNumeric().value();
        break;
    
    default:
        break;
    }
}

}
}