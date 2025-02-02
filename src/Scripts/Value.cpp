#include <BLIB/Scripts/Value.hpp>

#include "ScriptImpl.hpp"
#include <cmath>

namespace bl
{
namespace script
{
const std::unordered_map<std::string, Value::Builtin> Value::builtins = {
    std::make_pair("clear", &Value::clearValue),
    std::make_pair("append", &Value::appendValue),
    std::make_pair("resize", &Value::resizeValue),
    std::make_pair("insert", &Value::insertValue),
    std::make_pair("erase", &Value::eraseValue),
    std::make_pair("find", &Value::findValue),
    std::make_pair("keys", &Value::keysValue),
    std::make_pair("at", &Value::atValue),
    std::make_pair("length", &Value::lengthValue)};

const std::unordered_map<std::string, ReferenceValue> Value::EmptyProps;

Value::Value(const PrimitiveValue& v)
: _value(v) {}

Value::Value(const Value& v)
: std::enable_shared_from_this<Value>(v)
, _value(v._value) {
    if (v.properties) {
        properties.reset(new std::unordered_map<std::string, ReferenceValue>(*v.properties));
    }
}

Value::Value(Value&& v)
: std::enable_shared_from_this<Value>(v)
, _value(std::move(v._value)) {
    if (v.properties) { properties.swap(v.properties); }
}

Value::Value(PrimitiveValue&& v)
: _value(v) {}

Value& Value::operator=(PrimitiveValue&& v) {
    deref()._value = v;
    return *this;
}

Value& Value::operator=(const PrimitiveValue& v) {
    deref()._value = v;
    return *this;
}

Value& Value::operator=(const Value& val) {
    const Value& v = val.deref();
    Value& me      = deref();

    me._value = v._value;
    if (v.properties) {
        me.properties.reset(new std::unordered_map<std::string, ReferenceValue>(*v.properties));
    }
    return *this;
}

Value& Value::operator=(Value&& v) {
    Value& me = deref();

    me._value = std::move(v._value);
    if (v.properties) { me.properties.swap(v.properties); }
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

PrimitiveValue& Value::value() { return deref()._value; }

const PrimitiveValue& Value::value() const { return deref()._value; }

PrimitiveValue Value::noDerefValue() const {
    if (&deref() == this) {
        if (properties) { return {ReferenceValue(*this)}; }
    }
    return _value;
}

void Value::makeSafe() {
    if (_value.getType() == PrimitiveValue::TRef) { _value.getAsRef().makeSafe(); }
}

ReferenceValue Value::getProperty(const std::string& name, bool create) {
    auto& props = deref().properties;

    const auto bit = builtins.find(name);
    if (bit != builtins.end()) { return ReferenceValue((this->*bit->second)()); }

    if (!props && !create) { throw Error("Cannot access undefined property '" + name + "'"); }
    else if (!props) {
        props.reset(new std::unordered_map<std::string, ReferenceValue>());
    }

    const auto it = props->find(name);
    if (it == props->end()) {
        if (create) { return props->emplace(name, Value()).first->second; }
        throw Error("Cannot access undefined property '" + name + "'");
    }
    return it->second;
}

ReferenceValue Value::getProperty(const std::string& name, bool) const {
    const auto& props = deref().properties;

    const auto bit = builtins.find(name);
    if (bit != builtins.end()) {
        return ReferenceValue((const_cast<Value*>(this)->*bit->second)());
    }

    if (!props) { throw Error("Cannot access undefined property '" + name + "'"); }

    const auto it = props->find(name);
    if (it == props->end()) { throw Error("Cannot access undefined property '" + name + "'"); }
    return it->second;
}

void Value::setProperty(const std::string& name, const ReferenceValue& val) {
    if (builtins.find(name) != builtins.end()) {
        throw Error("Cannot write to reserved property '" + name + "'");
    }

    auto& props = deref().properties;
    if (!props) { props.reset(new std::unordered_map<std::string, ReferenceValue>()); }

    auto it = props->find(name);
    if (it != props->end()) { it->second = val; }
    else {
        props->emplace(name, val);
    }
}

const std::unordered_map<std::string, ReferenceValue>& Value::allProperties() const {
    const Value& me = deref();
    return me.properties ? *me.properties : EmptyProps;
}

void Value::clear(SymbolTable&, const std::vector<Value>&, Value&) { _value.getAsArray().clear(); }

Value Value::clearValue() {
    return Value(Function(std::bind(&Value::clear,
                                    &deref(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3)));
}

void Value::append(SymbolTable&, const std::vector<Value>& args, Value&) {
    ArrayValue& arr = _value.getAsArray();
    arr.reserve(arr.size() + args.size());
    for (const Value& v : args) { arr.emplace_back(v); }
}

Value Value::appendValue() {
    return Value(Function(std::bind(&Value::append,
                                    &deref(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3)));
}

void Value::insert(SymbolTable&, const std::vector<Value>& args, Value&) {
    ArrayValue& arr = _value.getAsArray();
    if (args.size() < 2) throw Error("insert() requires a position and a list of elements");
    const PrimitiveValue& i = args[0].value().deref();
    if (i.getType() != PrimitiveValue::TInteger)
        throw Error("First argument of insert() must be Integer");
    if (i.getAsInt() < 0) throw Error("Position in insert() must be positive");

    const unsigned int j = i.getAsInt();
    if (j >= arr.size()) throw Error("Position in insert() is out of bounds");

    arr.insert(arr.begin() + j, args.begin() + 1, args.end());
}

Value Value::insertValue() {
    return Value(Function(std::bind(&Value::insert,
                                    &deref(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3)));
}

void Value::erase(SymbolTable&, const std::vector<Value>& args, Value&) {
    ArrayValue& arr = _value.getAsArray();
    if (args.size() != 1) throw Error("erase() requires a position");
    const PrimitiveValue& i = args[0].value().deref();
    if (i.getType() != PrimitiveValue::TInteger)
        throw Error("First argument of erase() must be Numeric");
    if (i.getAsInt() < 0) throw Error("Position in erase() must be positive");

    const unsigned int j = i.getAsInt();
    if (j >= arr.size()) throw Error("Position in erase() is out of bounds");
    arr.erase(arr.begin() + j);
}

Value Value::eraseValue() {
    return Value(Function(std::bind(&Value::erase,
                                    &deref(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3)));
}

void Value::resize(SymbolTable&, const std::vector<Value>& args, Value&) {
    static const Value Default;

    ArrayValue& arr = _value.getAsArray();
    if (args.size() != 1 && args.size() != 2)
        throw Error("resize() expects a size and optional fill value");
    const PrimitiveValue& i = args[0].value().deref();
    if (i.getType() != PrimitiveValue::TInteger)
        throw Error("First argument of resize() must be Numeric");
    if (i.getAsInt() < 0) throw Error("Length in resize() must be positive");

    const unsigned int len = i.getAsInt();
    const Value& fill      = args.size() == 2 ? args[1] : Default;
    arr.resize(len, fill);
}

Value Value::resizeValue() {
    return Value(Function(std::bind(&Value::resize,
                                    &deref(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3)));
}

void Value::find(SymbolTable&, const std::vector<Value>& args, Value& result) {
    ArrayValue& arr = _value.getAsArray();
    if (args.size() != 1) throw Error("find() takes a single argument");
    for (unsigned int i = 0; i < arr.size(); ++i) {
        if (ScriptImpl::equals(arr[i], args.front())) {
            result = i;
            return;
        }
    }
    result = -1;
}

Value Value::findValue() {
    return Value(Function(std::bind(&Value::find,
                                    &deref(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3)));
}

void Value::keys(SymbolTable&, const std::vector<Value>& args, Value& result) {
    if (!args.empty()) throw Error("keys() expects 0 arguments");
    ArrayValue keys;
    if (properties) {
        keys.reserve(properties->size());
        for (const auto& prop : *properties) { keys.emplace_back(prop.first); }
    }
    result.value() = std::move(keys);
}

Value Value::keysValue() {
    return Value(Function(std::bind(&Value::keys,
                                    &deref(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3)));
}

void Value::at(SymbolTable&, const std::vector<Value>& args, Value& result) {
    if (args.size() != 1) throw Error("at() takes a single argument");
    const PrimitiveValue& n = args[0].value().deref();
    if (n.getType() != PrimitiveValue::TString) throw Error("at() expects a String key");
    result = getProperty(n.getAsString(), false);
}

Value Value::atValue() {
    return Value(Function(std::bind(&Value::at,
                                    &deref(),
                                    std::placeholders::_1,
                                    std::placeholders::_2,
                                    std::placeholders::_3)));
}

Value Value::lengthValue() {
    if (_value.deref().getType() != PrimitiveValue::TArray) {
        throw Error("Cannot access reserved property 'length' on non-array types");
    }
    return ReferenceValue(Value(_value.deref().getAsArray().size()));
}

} // namespace script
} // namespace bl
