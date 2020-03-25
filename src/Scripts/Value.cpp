#include <BLIB/Scripts/Value.hpp>

#include <BLIB/Scripts/Function.hpp>
#include <cmath>
#include <iostream>

namespace bl
{
namespace scripts
{
Value::Value()
: type(TVoid)
, value(new TData(0.0f)) {}

Value::Value(const Value& cpy) { *this = cpy; }

Value& Value::operator=(const Value& rhs) {
    type       = rhs.type;
    properties = rhs.properties;
    value.reset(new TData(*rhs.value.get()));
    return *this;
}

void Value::makeBool(bool val) {
    type = TBool;
    value.reset(new TData(val));
    resetProps();
}

Value& Value::operator=(float num) {
    type = TNumeric;
    value.reset(new TData(num));
    resetProps();
    return *this;
}

Value::Value(float num) { *this = num; }

Value& Value::operator=(const std::string& str) {
    type = TString;
    value.reset(new TData(str));
    resetProps();
    return *this;
}

Value::Value(const std::string& str) { *this = str; }

Value& Value::operator=(const Array& arr) {
    type = TArray;
    value.reset(new TData(arr));
    resetProps();
    return *this;
}

Value::Value(const Array& arr) { *this = arr; }

Value& Value::operator=(Ref ref) {
    type = TRef;
    value.reset(new TData(ref));
    resetProps();
    return *this;
}

Value::Value(Ref ref) { *this = ref; }

Value& Value::operator=(const Function& func) {
    type = TFunction;
    value.reset(new TData(func));
    resetProps();
    return *this;
}

Value::Value(const Function& func) { *this = func; }

Value::Type Value::getType() const { return type; }

bool Value::getAsBool() const {
    switch (type) {
    case TVoid:
        return false;
    case TBool: {
        const bool* b = std::get_if<bool>(value.get());
        return b ? *b : false;
    }
    case TNumeric:
        return getAsNum() != 0;
    case TString:
        return !getAsString().empty();
    case TArray:
        return !getAsArray().empty();
    case TRef:
        if (getAsRef().expired()) return false;
        return getAsRef().lock()->getAsBool();
    default:
        return false;
    }
}

float Value::getAsNum() const {
    const float* f = std::get_if<float>(value.get());
    if (f) return *f;
    return 0.0f;
}

std::string Value::getAsString() const {
    const std::string* s = std::get_if<std::string>(value.get());
    if (s) return *s;
    return "ERROR";
}

Value::Array Value::getAsArray() const {
    const Array* a = std::get_if<Array>(value.get());
    if (a) return *a;
    return {};
}

Value::Ref Value::getAsRef() {
    Ref* r = std::get_if<Ref>(value.get());
    if (r) return *r;
    return {};
}

Value::CRef Value::getAsRef() const {
    Ref* r = std::get_if<Ref>(value.get());
    if (r) return *r;
    return {};
}

Function Value::getAsFunction() const {
    const Function* func = std::get_if<Function>(value.get());
    return func ? *func : Function();
}

Value Value::getProperty(const std::string& name) const {
    if (name == "length" && type == TArray) return static_cast<int>(getAsArray().size());
    auto i = properties.find(name);
    if (i != properties.end()) return i->second;
    return {};
}

bool Value::setProperty(const std::string& name, const Value& val) {
    if (type == TArray && name == "length") {
        if (val.getType() != TNumeric) {
            std::cerr << "Assigning array length to non-numeric type";
            return false;
        }
        const float f = val.getAsNum();
        if (std::floor(f) != f) {
            std::cerr << "Array length must be an integer";
            return false;
        }
        Array& a = *std::get_if<Array>(value.get());
        a.resize(std::floor(f));
    }
    else
        properties[name] = val;
    return true;
}

void Value::resetProps() { properties.clear(); }

} // namespace scripts
} // namespace bl