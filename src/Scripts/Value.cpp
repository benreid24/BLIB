#include <BLIB/Scripts/Value.hpp>

namespace bl
{
namespace scripts
{
Value::Value()
: type(TVoid)
, value(0.0f) {}

Value& Value::operator=(float num) {
    type  = TNumeric;
    value = num;
    return *this;
}

Value& Value::operator=(const std::string& str) {
    type  = TString;
    value = str;
    return *this;
}

Value& Value::operator=(const Array& arr) {
    type  = TArray;
    value = arr;
    return *this;
}

Value& Value::operator=(Ref ref) {
    type  = TRef;
    value = ref;
    return *this;
}

Value& Value::operator=(const Function& func) {
    type  = TFunction;
    value = func;
    return *this;
}

Value::Type Value::getType() const { return type; }

float Value::getAsNum() const {
    const float* f = std::get_if<float>(&value);
    if (f) return *f;
    return 0.0f;
}

std::string Value::getAsString() const {
    const std::string* s = std::get_if<std::string>(&value);
    if (s) return *s;
    return "ERROR";
}

Value::Array Value::getAsArray() const {
    const Array* a = std::get_if<Array>(&value);
    if (a) return *a;
    return {};
}

Value::Ref Value::getAsRef() {
    Ref* r = std::get_if<Ref>(&value);
    if (r) return *r;
    return {};
}

Value::Ptr Value::getProperty(const std::string& name, bool create) {
    auto i = properties.find(name);
    if (i != properties.end())
        return i->second;
    else if (create)
        return properties.insert(std::make_pair(name, Ptr(new Value()))).first->second;
    return nullptr;
}

} // namespace scripts
} // namespace bl