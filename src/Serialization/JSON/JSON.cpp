#include <BLIB/Serialization/JSON/JSON.hpp>

#include <Serialization/JSON/JSONLoader.hpp>
#include <fstream>

namespace bl
{
namespace serial
{
namespace json
{
namespace
{
const Value* getNestedValue(const Group& group, std::string path) {
    const Group* nestedGroup = &group;

    while (!path.empty()) {
        const auto n = path.find_first_of('/');
        if (n != std::string::npos) {
            const std::string subname = path.substr(0, n);
            const Value* val          = nestedGroup->getField(subname);
            if (!val || !val->getAsGroup()) return nullptr;
            nestedGroup = val->getAsGroup();
            path.erase(0, n + 1);
        }
        else
            return nestedGroup->getField(path);
    }

    return nullptr;
}
} // namespace

void Base::setSource(const SourceInfo& source) { info = source; }

const SourceInfo& Base::source() const { return info; }

const std::set<std::string>& Group::getFields() const { return fieldNames; }

void Group::addField(const std::string& name, const Value& value) {
    fieldNames.insert(name);
    fields.insert(std::make_pair(name, value));
}

bool Group::hasField(const std::string& name) const { return fields.find(name) != fields.end(); }

const Value* Group::getField(const std::string& name) const {
    const auto i = fields.find(name);
    if (i != fields.end()) return &i->second;
    return nullptr;
}

const bool* Group::getBool(const std::string& name) const {
    const Value* val = getNestedValue(*this, name);
    if (val) return val->getAsBool();
    return nullptr;
}

long Group::getInteger(const std::string& name) const {
    const Value* val = getNestedValue(*this, name);
    if (val) return val->getNumericAsInteger();
    return 0;
}

float Group::getFloat(const std::string& name) const {
    const Value* val = getNestedValue(*this, name);
    if (val) return val->getNumericAsFloat();
    return 0.f;
}

const std::string* Group::getString(const std::string& name) const {
    const Value* val = getNestedValue(*this, name);
    if (val) return val->getAsString();
    return nullptr;
}

const Group* Group::getGroup(const std::string& name) const {
    const Value* val = getNestedValue(*this, name);
    if (val) return val->getAsGroup();
    return nullptr;
}

const List* Group::getList(const std::string& name) const {
    const Value* val = getNestedValue(*this, name);
    if (val) return val->getAsList();
    return nullptr;
}

Value* Group::getField(const std::string& name) {
    const auto i = fields.find(name);
    if (i != fields.end()) return &i->second;
    return nullptr;
}

bool* Group::getBool(const std::string& name) {
    Value* val = const_cast<Value*>(getNestedValue(*this, name));
    if (val) return val->getAsBool();
    return nullptr;
}

std::string* Group::getString(const std::string& name) {
    Value* val = const_cast<Value*>(getNestedValue(*this, name));
    if (val) return val->getAsString();
    return nullptr;
}

Group* Group::getGroup(const std::string& name) {
    Value* val = const_cast<Value*>(getNestedValue(*this, name));
    if (val) return val->getAsGroup();
    return nullptr;
}

List* Group::getList(const std::string& name) {
    Value* val = const_cast<Value*>(getNestedValue(*this, name));
    if (val) return val->getAsList();
    return nullptr;
}

Value::Value(const Value& value)
: type(value.type)
, data(value.data) {}

Value::Value(float value) { *this = value; }

Value::Value(const std::string& value) { *this = value; }

Value::Value(const char* value) { *this = std::string(value); }

Value::Value(const Group& value) { *this = value; }

Value::Value(const List& value) { *this = value; }

Value& Value::operator=(const Value& value) {
    type = value.type;
    data = value.data;
    return *this;
}

Value& Value::operator=(float value) {
    type = Type::Float;
    data = value;
    return *this;
}

Value& Value::operator=(const std::string& value) {
    type = Type::String;
    data = value;
    return *this;
}

Value& Value::operator=(const char* value) {
    *this = std::string(value);
    return *this;
}

Value& Value::operator=(const List& value) {
    type = Type::List;
    data = value;
    return *this;
}

Value& Value::operator=(const Group& value) {
    type = Type::Group;
    data = value;
    return *this;
}

Value::Type Value::getType() const { return type; }

const bool* Value::getAsBool() const { return std::get_if<bool>(&data); }

const long* Value::getAsInteger() const { return std::get_if<long>(&data); }

const float* Value::getAsFloat() const { return std::get_if<float>(&data); }

const std::string* Value::getAsString() const { return std::get_if<std::string>(&data); }

const Group* Value::getAsGroup() const { return std::get_if<Group>(&data); }

const List* Value::getAsList() const { return std::get_if<List>(&data); }

bool* Value::getAsBool() { return std::get_if<bool>(&data); }

long* Value::getAsInteger() { return std::get_if<long>(&data); }

float* Value::getAsFloat() { return std::get_if<float>(&data); }

std::string* Value::getAsString() { return std::get_if<std::string>(&data); }

Group* Value::getAsGroup() { return std::get_if<Group>(&data); }

List* Value::getAsList() { return std::get_if<List>(&data); }

float Value::getNumericAsFloat() const {
    switch (type) {
    case Type::Float:
        return *getAsFloat();
    case Type::Integer:
        return static_cast<float>(*getAsInteger());
    default:
        return 0.f;
    }
}

long Value::getNumericAsInteger() const {
    switch (type) {
    case Type::Float:
        return static_cast<float>(*getAsFloat());
    case Type::Integer:
        return *getAsInteger();
    default:
        return 0;
    }
}

std::ostream& operator<<(std::ostream& stream, const SourceInfo& info) {
    stream << info.filename << ":" << info.lineNumber;
    return stream;
}

void Group::print(std::ostream& stream, int ilvl) const {
    stream << "{";
    if (!fields.empty()) stream << "\n";
    for (auto i = fields.begin(); i != fields.end();) {
        stream << std::string(ilvl + 4, ' ');
        stream << '"' << i->first << "\": ";
        i->second.print(stream, ilvl + 4);
        if (++i != fields.end()) stream << ",\n";
    }
    stream << "\n" << std::string(ilvl, ' ') << "}";
}

void Value::print(std::ostream& stream, int ilvl) const {
    switch (getType()) {
    case Type::Bool:
        stream << (*getAsBool() ? "true" : "false");
        break;
    case Type::Float:
        stream << std::fixed << *getAsFloat();
        break;
    case Type::Integer:
        stream << *getAsInteger();
        break;
    case Type::String:
        stream << '"';
        for (char c : *getAsString()) {
            if (c == '"') stream << '\\';
            stream << c;
        }
        stream << '"';
        break;
    case Type::Group:
        getAsGroup()->print(stream, ilvl);
        break;
    case Type::List: {
        const List& list = *getAsList();
        stream << "[";
        if (!list.empty()) stream << "\n";
        for (unsigned int i = 0; i < list.size(); ++i) {
            stream << std::string(ilvl + 4, ' ');
            list[i].print(stream, ilvl + 4);
            if (i < list.size() - 1) stream << ",";
            stream << "\n";
        }
        stream << (list.empty() ? " " : std::string(ilvl, ' '));
        stream << "]";
        break;
    }

    default:
        break;
    }
}

std::ostream& operator<<(std::ostream& stream, const Value::Type& type) {
    switch (type) {
    case Value::Type::Bool:
        stream << "Bool";
        break;
    case Value::Type::String:
        stream << "String";
        break;
    case Value::Type::Integer:
        stream << "Integer";
        break;
    case Value::Type::Float:
        stream << "Float";
        break;
    case Value::Type::List:
        stream << "List";
        break;
    case Value::Type::Group:
        stream << "Group";
        break;
    default:
        stream << "UNKNOWN";
        break;
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Value& value) {
    value.print(stream, 0);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Group& group) {
    group.print(stream, 0);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const List& list) {
    stream << "[";
    if (!list.empty()) stream << "\n";
    for (unsigned int i = 0; i < list.size(); ++i) {
        list[i].print(stream, 4);
        if (i < list.size() - 1) stream << ",";
        stream << "\n";
    }
    stream << "]";
    return stream;
}

Group loadFromStream(std::istream& stream) {
    Loader loader(stream);
    return loader.load();
}

Group loadFromString(const std::string& data) {
    std::stringstream stream(data);
    return loadFromStream(stream);
}

Group loadFromFile(const std::string& file) {
    Loader loader(file);
    return loader.load();
}

void saveToFile(const std::string& file, const Group& group) {
    std::ofstream out(file.c_str());
    out << group;
}

void saveToStream(std::ostream& stream, const Group& group) { stream << group; }

} // namespace json
} // namespace serial
} // namespace bl
