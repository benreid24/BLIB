#include <BLIB/Scripts/PrimitiveValue.hpp>

#include <BLIB/Scripts/Value.hpp>

namespace bl
{
namespace script
{
std::string PrimitiveValue::typeToString(Type t) {
    switch (t) {
    case TVoid:
        return "Void";
    case TBool:
        return "Bool";
    case TInteger:
        return "Integer";
    case TFloat:
        return "Float";
    case TString:
        return "String";
    case TArray:
        return "Array";
    case TFunction:
        return "Function";
    case TRef:
        return "Reference";
    default: {
        std::string ret = "(";
        for (unsigned int i = 0; i < Type::_TYPE_COUNT; ++i) {
            const Type v = static_cast<Type>(0x1 << i);
            if ((t & v) != 0) { ret += typeToString(v) + ", "; }
        }
        ret.pop_back();
        ret.back() = ')';
        return ret;
    }
    }
}

PrimitiveValue::PrimitiveValue()
: type(TVoid)
, value(Empty()) {}

PrimitiveValue::PrimitiveValue(float n)
: type(TFloat)
, value(n) {}

PrimitiveValue::PrimitiveValue(ArrayValue&& array)
: type(TArray)
, value(array) {}

PrimitiveValue::PrimitiveValue(const ArrayValue& array)
: type(TArray)
, value(array) {}

PrimitiveValue::PrimitiveValue(const std::string& s)
: type(TString)
, value(s) {}

PrimitiveValue::PrimitiveValue(const char* c)
: type(TString) {
    value.emplace<std::string>(c);
}

PrimitiveValue::PrimitiveValue(const ReferenceValue& ref)
: type(TRef)
, value(ref) {}

PrimitiveValue::PrimitiveValue(const Function& f)
: type(TFunction)
, value(f) {}

PrimitiveValue& PrimitiveValue::operator=(float n) {
    type = TFloat;
    value.emplace<float>(n);
    return *this;
}

PrimitiveValue& PrimitiveValue::operator=(const std::string& s) {
    type = TString;
    value.emplace<std::string>(s);
    return *this;
}

PrimitiveValue& PrimitiveValue::operator=(const char* c) {
    type = TString;
    value.emplace<std::string>(c);
    return *this;
}

PrimitiveValue& PrimitiveValue::operator=(ArrayValue&& array) {
    type = TArray;
    value.emplace<ArrayValue>(array);
    return *this;
}

PrimitiveValue& PrimitiveValue::operator=(const ReferenceValue& ref) {
    type = TRef;
    value.emplace<ReferenceValue>(ref);
    return *this;
}

PrimitiveValue& PrimitiveValue::operator=(const Function& f) {
    type = TFunction;
    value.emplace<Function>(f);
    return *this;
}

const PrimitiveValue& PrimitiveValue::deref() const {
    return type == TRef ? getAsRef().deref().value() : *this;
}

PrimitiveValue::Type PrimitiveValue::getType() const { return type; }

bool PrimitiveValue::getAsBool() const {
    switch (type) {
    case TVoid:
        return false;
    case TBool:
        return *std::get_if<bool>(&value);
    case TInteger:
        return getAsInt() != 0;
    case TFloat:
        return getAsFloat() != 0.f;
    case TString:
        return !getAsString().empty();
    case TArray:
        return !getAsArray().empty();
    case TRef:
        return getAsRef().deref().value().getAsBool();
    default:
        return false;
    }
}

long PrimitiveValue::getAsInt() const {
    const long* l = std::get_if<long>(&value);
    if (!l) throw Error("Accessing non-integer value as integer");
    return *l;
}

long PrimitiveValue::getNumAsInt() const {
    const long* l = std::get_if<long>(&value);
    if (l) return *l;
    const float* f = std::get_if<float>(&value);
    if (f) return static_cast<long>(*f);
    throw Error("Accessing non-numeric value as numeric");
}

float PrimitiveValue::getAsFloat() const {
    const float* f = std::get_if<float>(&value);
    if (!f) throw Error("Accessing non-float value as float");
    return *f;
}

float PrimitiveValue::getNumAsFloat() const {
    const float* f = std::get_if<float>(&value);
    if (f) return *f;
    const long* l = std::get_if<long>(&value);
    if (l) return static_cast<float>(*l);
    throw Error("Accessing non-numeric value as numeric");
}

const ArrayValue& PrimitiveValue::getAsArray() const {
    const ArrayValue* a = std::get_if<ArrayValue>(&value);
    if (!a) throw Error("Accessing non-array value as array");
    return *a;
}

ArrayValue& PrimitiveValue::getAsArray() {
    ArrayValue* a = std::get_if<ArrayValue>(&value);
    if (!a) throw Error("Accessing non-array value as array");
    return *a;
}

const std::string& PrimitiveValue::getAsString() const {
    const std::string* s = std::get_if<std::string>(&value);
    if (!s) throw Error("Accessing non-string value as string");
    return *s;
}

const Function& PrimitiveValue::getAsFunction() const {
    const Function* f = std::get_if<Function>(&value);
    if (!f) throw Error("Accessing non-function value as function");
    return *f;
}

const ReferenceValue& PrimitiveValue::getAsRef() const {
    const ReferenceValue* r = std::get_if<ReferenceValue>(&value);
    if (!r) throw Error("Accessing non-reference value as reference");
    return *r;
}

ReferenceValue& PrimitiveValue::getAsRef() {
    ReferenceValue* r = std::get_if<ReferenceValue>(&value);
    if (!r) throw Error("Accessing non-reference value as reference");
    return *r;
}

} // namespace script
} // namespace bl
