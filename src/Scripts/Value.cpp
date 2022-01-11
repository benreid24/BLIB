#include <BLIB/Scripts/Value.hpp>

#include "ScriptImpl.hpp"
#include <cmath>

namespace bl
{
namespace script
{
const std::unordered_map<std::string, Value::Builtin> Value::builtins = {
    std::make_pair("clear", &Value::clear),
    std::make_pair("append", &Value::append),
    std::make_pair("resize", &Value::resize),
    std::make_pair("insert", &Value::insert),
    std::make_pair("erase", &Value::erase),
    std::make_pair("find", &Value::find),
    std::make_pair("keys", &Value::keys),
    std::make_pair("at", &Value::at)};

Value::Value(const PrimitiveValue& v)
: _value(v) {}

Value::Value(PrimitiveValue&& v)
: _value(v) {}

Value& Value::operator=(PrimitiveValue&& v) {
    _value = v;
    return *this;
}

Value& Value::operator=(const PrimitiveValue& v) {
    _value = v;
    return *this;
}

ReferenceValue Value::getRef() const {
    try {
        std::shared_ptr<Value> me = const_cast<Value*>(this)->shared_from_this();
        return ReferenceValue(std::move(me));
    } catch (std::bad_weak_ptr&) { return ReferenceValue(const_cast<Value*>(this)); }
}

Value& Value::deref() {
    return _value.getType() == PrimitiveValue::TRef ? _value.getAsRef().deref() : *this;
}

const Value& Value::deref() const {
    return _value.getType() == PrimitiveValue::TRef ? _value.getAsRef().deref() : *this;
}

PrimitiveValue& Value::value() { return _value; }

const PrimitiveValue& Value::value() const { return _value; }

ReferenceValue Value::getProperty(const std::string& name) {
    auto it        = properties.find(name);
    const auto bit = builtins.find(name);

    if (bit != builtins.end()) {
        if (it == properties.end()) {
            it = properties
                     .emplace(name,
                              Function(std::bind(
                                  bit->second, this, std::placeholders::_1, std::placeholders::_2)))
                     .first;
        }
        return it->second;
    }

    if (name == "length") {
        if (_value.deref().getType() != PrimitiveValue::TArray) {
            throw Error("Cannot access reserved property 'length' on non-array types");
        }
        return ReferenceValue(_value.deref().getAsArray().size());
    }

    if (it == properties.end()) { throw Error("Cannot access undefined property '" + name + "'"); }

    return it->second;
}

ReferenceValue Value::getProperty(const std::string& name) const {
    auto it = properties.find(name);
    if (it == properties.end()) { throw Error("Cannot access undefined property '" + name + "'"); }
    return it->second;
}

void Value::setProperty(const std::string& name, const ReferenceValue& val) {
    if (builtins.find(name) != builtins.end() || name == "length") {
        throw Error("Cannot write to reserved property '" + name + "'");
    }
    auto it = properties.find(name);
    if (it != properties.end()) { it->second = val; }
    else {
        properties.emplace(name, val);
    }
}

void Value::getAllKeys(std::vector<std::string>& keys) const {
    keys.reserve(properties.size());
    for (const auto& prop : properties) {
        if (builtins.find(prop.first) == builtins.end()) { keys.emplace_back(prop.first); }
    }
}

Value Value::clear(SymbolTable&, const std::vector<Value>&) {
    _value.getAsArray().clear();
    return {};
}

Value Value::append(SymbolTable&, const std::vector<Value>& args) {
    ArrayValue& arr = _value.getAsArray();
    arr.reserve(arr.size() + args.size());
    for (const Value& v : args) { arr.emplace_back(v); }
    return {};
}

Value Value::insert(SymbolTable&, const std::vector<Value>& args) {
    ArrayValue& arr = _value.getAsArray();
    if (args.size() < 2) throw Error("insert() requires a position and a list of elements");
    const PrimitiveValue& i = args[0].value().deref();
    if (i.getType() != PrimitiveValue::TNumeric)
        throw Error("First argument of insert() must be Numeric");
    if (std::floor(i.getAsNum()) != i.getAsNum())
        throw Error("Position in insert() must be an integer");
    if (i.getAsNum() < 0) throw Error("Position in insert() must be positive");

    const unsigned int j = i.getAsNum();
    if (j >= arr.size()) throw Error("Position in insert() is out of bounds");

    arr.insert(arr.begin() + j, args.begin() + 1, args.end());
    return {};
}

Value Value::erase(SymbolTable&, const std::vector<Value>& args) {
    ArrayValue& arr = _value.getAsArray();
    if (args.size() != 1) throw Error("erase() requires a position");
    const PrimitiveValue& i = args[0].value().deref();
    if (i.getType() != PrimitiveValue::TNumeric)
        throw Error("First argument of erase() must be Numeric");
    if (std::floor(i.getAsNum()) != i.getAsNum())
        throw Error("Position in erase() must be an integer");
    if (i.getAsNum() < 0) throw Error("Position in erase() must be positive");

    const unsigned int j = i.getAsNum();
    if (j >= arr.size()) throw Error("Position in erase() is out of bounds");
    arr.erase(arr.begin() + j);
    return {};
}

Value Value::resize(SymbolTable&, const std::vector<Value>& args) {
    static const Value Default;

    ArrayValue& arr = _value.getAsArray();
    if (args.size() != 1 && args.size() != 2)
        throw Error("resize() expects a size and optional fill value");
    const PrimitiveValue& i = args[0].value().deref();
    if (i.getType() != PrimitiveValue::TNumeric)
        throw Error("First argument of resize() must be Numeric");
    if (std::floor(i.getAsNum()) != i.getAsNum())
        throw Error("Length in resize() must be an integer");
    if (i.getAsNum() < 0) throw Error("Length in resize() must be positive");

    const unsigned int len = i.getAsNum();
    const Value& fill      = args.size() == 2 ? args[1] : Default;
    arr.resize(len, fill);
    return {};
}

Value Value::find(SymbolTable&, const std::vector<Value>& args) {
    ArrayValue& arr = _value.getAsArray();
    if (args.size() != 1) throw Error("find() takes a single argument");
    float i = 0.f;
    for (const Value& element : arr) {
        if (ScriptImpl::equals(element, args.front())) { return Value(i); }
        i += 1.f;
    }
    return Value(-1.f);
}

Value Value::keys(SymbolTable&, const std::vector<Value>& args) {
    if (!args.empty()) throw Error("keys() expects 0 arguments");
    ArrayValue keys;
    keys.reserve(properties.size());
    for (const auto& prop : properties) {
        if (builtins.find(prop.first) == builtins.end() && prop.first != "length") {
            keys.emplace_back(prop.first);
        }
    }
    return {PrimitiveValue(std::move(keys))};
}

Value Value::at(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("at() takes a single argument");
    const PrimitiveValue& n = args[0].value().deref();
    if (n.getType() != PrimitiveValue::TString) throw Error("at() expects a String key");
    return {getProperty(n.getAsString())};
}

} // namespace script
} // namespace bl
