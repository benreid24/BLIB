#include <BLIB/Scripts/Value.hpp>

#include <BLIB/Scripts/Error.hpp>
#include <BLIB/Scripts/Function.hpp>
#include <cmath>
#include <functional>

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

Value& Value::operator=(const std::vector<Value>& a) {
    std::vector<Ptr> array;
    array.reserve(a.size());
    for (const Value& v : a) array.push_back(Ptr(new Value(v)));
    *this = array;
    return *this;
}

Value::Value(const std::vector<Value>& a) { *this = a; }

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

Value::Ptr Value::getProperty(const std::string& name, bool create) {
    if (type == TArray) {
        using namespace std::placeholders;
        if (name == "length")
            return Ptr(new Value(static_cast<int>(getAsArray().size())));
        else if (name == "resize") {
            Function::CustomCB cb = [this](SymbolTable&,
                                           const std::vector<Value>& args) -> Value {
                this->resize(args);
                return Value();
            };
            return Ptr(new Value(cb));
        }
        else if (name == "append") {
            Function::CustomCB cb = [this](SymbolTable&,
                                           const std::vector<Value>& args) -> Value {
                this->append(args);
                return Value();
            };
            return Ptr(new Value(cb));
        }
        else if (name == "insert") {
            Function::CustomCB cb = [this](SymbolTable&,
                                           const std::vector<Value>& args) -> Value {
                this->insert(args);
                return Value();
            };
            return Ptr(new Value(cb));
        }
        else if (name == "clear") {
            Function::CustomCB cb = [this](SymbolTable&, const std::vector<Value>&) -> Value {
                this->clear();
                return Value();
            };
            return Ptr(new Value(cb));
        }
        else if (name == "erase") {
            Function::CustomCB cb = [this](SymbolTable&,
                                           const std::vector<Value>& args) -> Value {
                this->erase(args);
                return Value();
            };
            return Ptr(new Value(cb));
        }
    }
    if (name == "keys") {
        Function::CustomCB cb = [this](SymbolTable&, const std::vector<Value>& args) -> Value {
            return this->keys(args);
        };
        return Ptr(new Value(cb));
    }
    else if (name == "at") {
        Function::CustomCB cb = [this](SymbolTable&, const std::vector<Value>& args) -> Value {
            return this->at(args);
        };
        return Ptr(new Value(cb));
    }
    auto i = properties.find(name);
    if (i != properties.end()) return i->second;
    if (create) {
        properties[name] = Ptr(new Value());
        return properties[name];
    }
    return {};
}

bool Value::setProperty(const std::string& name, const Value& val) {
    if (name == "length" || name == "resize" || name == "append" || name == "insert" ||
        name == "clear" || name == "erase" || name == "keys" || name == "at")
        return false;
    Ptr v = getProperty(name);
    if (!v) {
        v                = Ptr(new Value(val));
        properties[name] = v;
    }
    else
        *v = val;
    return true;
}

void Value::resetProps() { properties.clear(); }

void Value::clear() {
    Value::Array* arr = std::get_if<Value::Array>(value.get());
    if (arr) arr->clear();
}

void Value::append(const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(value.get());
    if (arr) {
        for (const Value& v : args) { arr->push_back(Ptr(new Value(v))); }
    }
}

void Value::insert(const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(value.get());
    if (arr) {
        if (args.size() < 2)
            throw Error("insert() requires a position and a list of elements");
        if (args[0].getType() != TNumeric)
            throw Error("First argument of insert() must be Numeric");
        if (std::floor(args[0].getAsNum()) != args[0].getAsNum())
            throw Error("Position in insert() must be an integer");
        if (args[0].getAsNum() < 0) throw Error("Position in insert() must be positive");

        const unsigned int i = args[0].getAsNum();
        if (i >= arr->size()) throw Error("Position in insert() is out of bounds");

        std::vector<Ptr> ins;
        ins.reserve(args.size() - 1);
        for (unsigned int j = 1; j < args.size(); ++j) ins.push_back(Ptr(new Value(args[j])));
        arr->insert(arr->begin() + i, ins.begin(), ins.end());
    }
}

void Value::erase(const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(value.get());
    if (arr) {
        if (args.size() != 1) throw Error("erase() requires a position");
        if (args[0].getType() != TNumeric)
            throw Error("First argument of erase() must be Numeric");
        if (std::floor(args[0].getAsNum()) != args[0].getAsNum())
            throw Error("Position in erase() must be an integer");
        if (args[0].getAsNum() < 0) throw Error("Position in erase() must be positive");

        const unsigned int i = args[0].getAsNum();
        if (i >= arr->size()) throw Error("Position in erase() is out of bounds");
        arr->erase(arr->begin() + i);
    }
}

void Value::resize(const std::vector<Value>& args) {
    Value::Array* arr = std::get_if<Value::Array>(value.get());
    if (arr) {
        if (args.size() != 1 && args.size() != 2)
            throw Error("resize() expects a size and optional fill value");
        if (args[0].getType() != TNumeric)
            throw Error("First argument of resize() must be Numeric");
        if (std::floor(args[0].getAsNum()) != args[0].getAsNum())
            throw Error("Length in resize() must be an integer");
        if (args[0].getAsNum() < 0) throw Error("Length in resize() must be positive");

        const unsigned int len = args[0].getAsNum();
        if (len < arr->size())
            arr->resize(len);
        else {
            arr->reserve(len);
            Value fill;
            if (args.size() == 2) fill = args[1];
            unsigned int s = len - arr->size();
            for (unsigned int i = 0; i < s; ++i) { arr->push_back(Ptr(new Value(fill))); }
        }
    }
}

Value Value::keys(const std::vector<Value>& args) {
    if (!args.empty()) throw Error("keys() expects 0 arguments");
    Value::Array keys;
    keys.reserve(properties.size());
    for (auto i = properties.begin(); i != properties.end(); ++i) {
        keys.push_back(Ptr(new Value(i->first)));
    }
    return keys;
}

Value Value::at(const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("at() takes a single argument");
    if (args[0].getType() != TString) throw Error("at() expects a String key");
    Value::Ptr p = getProperty(args[0].getAsString(), false);
    if (p) return *p;
    return Value();
}

} // namespace scripts
} // namespace bl