#include <BLIB/Scripts/Value.hpp>

#include <BLIB/Scripts/Error.hpp>
#include <BLIB/Scripts/Function.hpp>
#include <Scripts/ScriptImpl.hpp>
#include <cmath>
#include <functional>

namespace bl
{
namespace script
{
const std::unordered_map<std::string, Value::Builtin> Value::Builtins = {
    std::make_pair("clear", &Value::clear),
    std::make_pair("append", &Value::append),
    std::make_pair("resize", &Value::resize),
    std::make_pair("insert", &Value::insert),
    std::make_pair("erase", &Value::erase),
    std::make_pair("find", &Value::find),
    std::make_pair("keys", &Value::keys),
    std::make_pair("at", &Value::at)};

std::string Value::typeToString(Type t) {
    switch (t) {
    case TVoid:
        return "Void";
    case TBool:
        return "Bool";
    case TNumeric:
        return "Numeric";
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
        for (unsigned int i = 0; i < Value::_TYPE_COUNT; ++i) {
            const Type v = static_cast<Type>(0x1 << i);
            if ((t & v) != 0) { ret += typeToString(v) + ", "; }
        }
        ret.pop_back();
        ret.back() = ')';
        return ret;
    }
    }
}

Value::Value()
: type(TVoid)
, value(Empty())
, depth(-1) {}

Value::Value(float num)
: type(TNumeric)
, value(num)
, depth(-1) {}

Value::Value(const std::string& str)
: type(TString)
, value(str)
, depth(-1) {}

Value::Value(const Array& arr)
: type(TArray)
, value(arr)
, depth(-1) {}

Value::Value(const Ref& ref)
: type(TRef)
, value(ref)
, depth(-1) {}

Value::Value(const Function& func)
: type(TFunction)
, value(func)
, depth(-1) {}

Value& Value::operator=(float num) {
    type = TNumeric;
    value.emplace<float>(num);
    return *this;
}

Value& Value::operator=(const std::string& str) {
    type = TString;
    value.emplace<std::string>(str);
    return *this;
}

Value& Value::operator=(const Array& arr) {
    type = TArray;
    value.emplace<Array>(arr);
    return *this;
}

Value& Value::operator=(const Ref& ref) {
    type = TRef;
    value.emplace<Ref>(ref);
    return *this;
}

Value& Value::operator=(const Function& func) {
    type = TFunction;
    value.emplace<Function>(func);
    return *this;
}

Value::Type Value::getType() const { return type; }

bool Value::getAsBool(int d) const {
    switch (type) {
    case TVoid:
        return false;
    case TBool: {
        const bool* b = std::get_if<bool>(&value);
        return b ? *b : false;
    }
    case TNumeric:
        return getAsNum() != 0;
    case TString:
        return !getAsString().empty();
    case TArray:
        return !getAsArray().empty();
    case TRef:
        return deref(d).getAsBool();
    default:
        return false;
    }
}

float Value::getAsNum() const {
    const float* f = std::get_if<float>(&value);
    if (f) return *f;
    throw Error("Interpreter bug: Accessing float value of non-float type");
}

const std::string& Value::getAsString() const {
    const std::string* s = std::get_if<std::string>(&value);
    if (s) return *s;
    throw Error("Interpreter bug: Accessing string value of non-string type");
}

const Value::Array& Value::getAsArray() const {
    const Array* a = std::get_if<Array>(&value);
    if (a) return *a;
    throw Error("Interpreter bug: Accessing array value of non-array type");
}

Value::Array& Value::getAsArray() {
    Array* a = std::get_if<Array>(&value);
    if (a) return *a;
    throw Error("Interpreter bug: Accessing array value of non-array type");
}

const Value::Ref& Value::getAsRef() const {
    const Ref* r = std::get_if<Ref>(&value);
    if (r) return *r;
    throw Error("Interpreter bug: Accessing ref type value of non-ref type");
}

Value& Value::deref(int depth) {
    if (type == TRef) {
        const Ref& ref = getAsRef();
        if (ref.depth <= depth) return ref.value->deref(depth);

        throw Error("Dereferenced expired reference");
    }
    return *this;
}

const Value& Value::deref(int depth) const {
    if (type == TRef) {
        const auto& ref = getAsRef();
        if (ref.depth <= depth) return ref.value->deref(depth);

        throw Error("Dereferenced expired reference");
    }
    return *this;
}

const Function& Value::getAsFunction() const {
    const Function* func = std::get_if<Function>(&value);
    if (!func) throw Error("Internal error: Function access on non-function type");
    return *func;
}

Value Value::clear(SymbolTable&, const std::vector<Value>&) {
    Value::Array* arr = std::get_if<Value::Array>(&value);
    if (arr) arr->clear();
    return {};
}

Value Value::append(SymbolTable&, const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(&value);
    if (arr) {
        arr->reserve(arr->size() + args.size());
        for (const Value& v : args) { arr->emplace_back(v); }
    }
    return {};
}

Value Value::insert(SymbolTable&, const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(&value);
    if (arr) {
        if (args.size() < 2) throw Error("insert() requires a position and a list of elements");
        if (args[0].getType() != TNumeric)
            throw Error("First argument of insert() must be Numeric");
        if (std::floor(args[0].getAsNum()) != args[0].getAsNum())
            throw Error("Position in insert() must be an integer");
        if (args[0].getAsNum() < 0) throw Error("Position in insert() must be positive");

        const unsigned int i = args[0].getAsNum();
        if (i >= arr->size()) throw Error("Position in insert() is out of bounds");

        arr->insert(arr->begin() + i, args.begin() + 1, args.end());
    }
    return {};
}

Value Value::erase(SymbolTable&, const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(&value);
    if (arr) {
        if (args.size() != 1) throw Error("erase() requires a position");
        if (args[0].getType() != TNumeric) throw Error("First argument of erase() must be Numeric");
        if (std::floor(args[0].getAsNum()) != args[0].getAsNum())
            throw Error("Position in erase() must be an integer");
        if (args[0].getAsNum() < 0) throw Error("Position in erase() must be positive");

        const unsigned int i = args[0].getAsNum();
        if (i >= arr->size()) throw Error("Position in erase() is out of bounds");
        arr->erase(arr->begin() + i);
    }
    return {};
}

Value Value::resize(SymbolTable&, const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(&value);
    if (arr) {
        if (args.size() != 1 && args.size() != 2)
            throw Error("resize() expects a size and optional fill value");
        if (args[0].getType() != TNumeric)
            throw Error("First argument of resize() must be Numeric");
        if (std::floor(args[0].getAsNum()) != args[0].getAsNum())
            throw Error("Length in resize() must be an integer");
        if (args[0].getAsNum() < 0) throw Error("Length in resize() must be positive");

        const unsigned int len = args[0].getAsNum();
        arr->resize(len);
    }
    return {};
}

Value Value::find(SymbolTable& table, const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(&value);
    if (arr) {
        if (args.size() != 1) throw Error("find() takes a single argument");
        float i = 0.f;
        for (const Value& element : *arr) {
            if (ScriptImpl::equals(element, args.front(), table.currentDepth())) {
                return Value(i);
            }
            i += 1.f;
        }
    }
    return Value(-1.f);
}

Value Value::keys(SymbolTable& table, const std::vector<Value>& args) {
    if (!args.empty()) throw Error("keys() expects 0 arguments");
    const auto& props = table.getAllProps(*this);
    Value::Array keys;
    keys.reserve(props.size());
    for (const auto& prop : props) { keys.emplace_back(prop.first); }
    return {keys};
}

Value Value::at(SymbolTable& table, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("at() takes a single argument");
    if (args[0].getType() != TString) throw Error("at() expects a String key");
    Value* p = table.getProp({this, depth}, args[0].getAsString(), false);
    if (p) return *p;
    return Value();
}

} // namespace script
} // namespace bl
