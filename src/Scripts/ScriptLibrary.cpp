#include <Scripts/ScriptLibrary.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Scripts.hpp>
#include <BLIB/Util.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <sstream>
#include <thread>

namespace bl
{
namespace script
{
namespace
{
constexpr PrimitiveValue::Type TNumeric = PrimitiveValue::TFloat | PrimitiveValue::TInteger;

void print(SymbolTable&, const std::vector<Value>& args, Value& result);
void loginfo(SymbolTable&, const std::vector<Value>& args, Value& result);
void logerror(SymbolTable&, const std::vector<Value>& args, Value& result);
void logdebug(SymbolTable&, const std::vector<Value>& args, Value& result);
void randomInt(SymbolTable&, const std::vector<Value>& args, Value& result);
void randomFloat(SymbolTable&, const std::vector<Value>& args, Value& result);
void sleep(SymbolTable&, const std::vector<Value>& args, Value& result);
void time(SymbolTable&, const std::vector<Value>& args, Value& result);
void run(SymbolTable&, const std::vector<Value>& args, Value& result);
void exit(SymbolTable&, const std::vector<Value>& args, Value& result);
void error(SymbolTable&, const std::vector<Value>& args, Value& result);
void str(SymbolTable&, const std::vector<Value>& args, Value& result);
void toFloat(SymbolTable&, const std::vector<Value>& args, Value& result);
void toInt(SymbolTable&, const std::vector<Value>& args, Value& result);
void sqrt(SymbolTable&, const std::vector<Value>& args, Value& result);
void abs(SymbolTable&, const std::vector<Value>& args, Value& result);
void round(SymbolTable&, const std::vector<Value>& args, Value& result);
void floor(SymbolTable&, const std::vector<Value>& args, Value& result);
void ceil(SymbolTable&, const std::vector<Value>& args, Value& result);
void sin(SymbolTable&, const std::vector<Value>& args, Value& result);
void cos(SymbolTable&, const std::vector<Value>& args, Value& result);
void tan(SymbolTable&, const std::vector<Value>& args, Value& result);
void atan2(SymbolTable&, const std::vector<Value>& args, Value& result);
void min(SymbolTable&, const std::vector<Value>& args, Value& result);
void max(SymbolTable&, const std::vector<Value>& args, Value& result);
void sum(SymbolTable&, const std::vector<Value>& args, Value& result);
void typeofStr(SymbolTable&, const std::vector<Value>& args, Value& result);

typedef std::pair<std::string, Function::CustomCB> builtin;
const std::vector<builtin> builtins = {builtin("print", print),
                                       builtin("loginfo", loginfo),
                                       builtin("logdebug", logdebug),
                                       builtin("logerror", logerror),
                                       builtin("typeof", typeofStr),
                                       builtin("randomInt", randomInt),
                                       builtin("randomFloat", randomFloat),
                                       builtin("int", toInt),
                                       builtin("float", toFloat),
                                       builtin("sleep", sleep),
                                       builtin("time", time),
                                       builtin("run", run),
                                       builtin("exit", exit),
                                       builtin("error", error),
                                       builtin("str", str),
                                       builtin("sqrt", sqrt),
                                       builtin("abs", abs),
                                       builtin("round", round),
                                       builtin("floor", floor),
                                       builtin("ceil", ceil),
                                       builtin("sin", sin),
                                       builtin("cos", cos),
                                       builtin("tan", tan),
                                       builtin("atan2", atan2),
                                       builtin("min", min),
                                       builtin("max", max),
                                       builtin("sum", sum)};

bool isInt(const std::string& s) {
    for (unsigned int i = 0; i < s.size(); ++i) {
        if (!std::isdigit(s[i])) {
            if (s[i] == '-' && i == 0) continue;
            return false;
        }
    }
    return true;
}

bool isFloat(const std::string& s) {
    bool dot = false;
    for (unsigned int i = 0; i < s.size(); ++i) {
        if (!std::isdigit(s[i])) {
            if (s[i] == '-' && i == 0) continue;
            if (s[i] == '.' && !dot) {
                dot = true;
                continue;
            }
            return false;
        }
    }
    return true;
}

} // namespace

void Library::addBuiltIns(SymbolTable& table) {
    for (const builtin& b : builtins) { table.set(b.first, Value(Function(b.second))); }
}

namespace
{
std::string argsToStr(SymbolTable& t, const std::vector<Value>& args) {
    std::stringstream ss;
    for (const Value& v : args) {
        Value s;
        str(t, {v}, s);
        ss << s.value().getAsString();
    }
    return ss.str();
}

void print(SymbolTable& t, const std::vector<Value>& args, Value&) {
    std::cout << argsToStr(t, args) << std::endl;
}

void loginfo(SymbolTable& t, const std::vector<Value>& args, Value&) {
    bl::logging::Logger::info() << argsToStr(t, args);
}

void logerror(SymbolTable& t, const std::vector<Value>& args, Value&) {
    bl::logging::Logger::error() << argsToStr(t, args);
}

void logdebug(SymbolTable& t, const std::vector<Value>& args, Value&) {
    bl::logging::Logger::debug() << argsToStr(t, args);
}

void typeofStr(SymbolTable&, const std::vector<Value>& args, Value& result) {
    if (args.size() != 1) throw Error("typeof expects 1 argument");
    result = PrimitiveValue::typeToString(args.front().value().getType());
}

void randomFloat(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TFloat, PrimitiveValue::TFloat>("randomFloat", args);

    const float l = args[0].value().getAsFloat();
    const float r = args[1].value().getAsFloat();
    result        = util::Random::get(std::min(l, r), std::max(l, r));
}

void randomInt(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TInteger, PrimitiveValue::TInteger>("randomInt", args);

    const long l = args[0].value().getAsInt();
    const long r = args[1].value().getAsInt();
    result       = util::Random::get(std::min(l, r), std::max(l, r));
}

void toInt(SymbolTable&, const std::vector<Value>& args, Value& result) {
    if (args.size() != 1) throw Error("'int' expects 1 argument");

    switch (args[0].value().getType()) {
    case PrimitiveValue::TBool:
        result = (args[0].value().getAsBool() ? 1 : 0);
        break;
    case PrimitiveValue::TInteger:
        result = args[0].value().getAsInt();
        break;
    case PrimitiveValue::TFloat:
        result = static_cast<long>(args[0].value().getAsFloat());
        break;
    case PrimitiveValue::TString:
        if (!isInt(args[0].value().getAsString())) {
            throw Error("Invalid string for integer conversion: " + args[0].value().getAsString());
        }
        result = std::atol(args[0].value().getAsString().c_str());
        break;
    default:
        throw Error("Invalid type for integer conversion: " +
                    PrimitiveValue::typeToString(args[0].value().getType()));
    }
}

void toFloat(SymbolTable&, const std::vector<Value>& args, Value& result) {
    if (args.size() != 1) throw Error("'float' expects 1 argument");

    switch (args[0].value().getType()) {
    case PrimitiveValue::TBool:
        result = (args[0].value().getAsBool() ? 1.f : 0.f);
        break;
    case PrimitiveValue::TInteger:
        result = static_cast<float>(args[0].value().getAsInt());
        break;
    case PrimitiveValue::TFloat:
        result = args[0].value().getAsFloat();
        break;
    case PrimitiveValue::TString:
        if (!isFloat(args[0].value().getAsString())) {
            throw Error("Invalid string for float conversion: " + args[0].value().getAsString());
        }
        result = std::atof(args[0].value().getAsString().c_str());
        break;
    default:
        throw Error("Invalid type for float conversion: " +
                    PrimitiveValue::typeToString(args[0].value().getType()));
    }
}

void sleep(SymbolTable& table, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TInteger>("sleep", args);

    if (args[0].value().getAsInt() <= 0) throw Error("sleep() must be given a positive value");
    const unsigned int ms = args[0].value().getAsInt();
    table.waitFor(ms);
    if (table.killed()) throw Error("Script killed");
}

void time(SymbolTable&, const std::vector<Value>& args, Value& result) {
    if (!args.empty()) throw Error("time() takes 0 arguments");
    result = static_cast<float>(util::Timer::get().timeElapsedRaw().asMilliseconds());
}

void run(SymbolTable& table, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TString, PrimitiveValue::TBool>("run", args);

    Script script(args[0].value().getAsString(), table.base());
    if (!script.valid()) throw Error("Syntax error in script passed to run()");

    if (args[1].value().getAsBool()) {
        script.runBackground(table.manager());
        result = true;
    }
    else {
        const std::optional<Value> r = script.run(table.manager());
        result                       = r.value_or(Value());
    }
}

void exit(SymbolTable&, const std::vector<Value>& args, Value&) {
    if (!args.empty()) throw Error("exit() takes 0 arguments");
    throw Exit();
}

void error(SymbolTable&, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TString>("error", args);
    throw Error(args[0].value().getAsString());
}

void str(SymbolTable& t, const std::vector<Value>& args, Value& result) {
    if (args.size() != 1) throw Error("str() takes 1 argument");

    switch (args[0].value().getType()) {
    case PrimitiveValue::TBool:
        result = (args[0].value().getAsBool() ? "true" : "false");
        break;
    case PrimitiveValue::TInteger:
        result = std::to_string(args[0].value().getAsInt());
        break;
    case PrimitiveValue::TFloat:
        result = std::to_string(args[0].value().getAsFloat());
        break;
    case PrimitiveValue::TString:
        result = args[0].value().getAsString();
        break;
    case PrimitiveValue::TArray: {
        std::string s = "[";
        if (!args[0].value().getAsArray().empty()) {
            Value sv;
            str(t, {args[0].value().getAsArray()[0]}, sv);
            s += sv.value().getAsString();
        }
        for (unsigned int i = 1; i < args[0].value().getAsArray().size(); ++i) {
            Value sv;
            str(t, {args[0].value().getAsArray()[i]}, sv);
            s += ", " + sv.value().getAsString();
        }
        s += "]";
        result = s;
        break;
    }
    case PrimitiveValue::TRef:
        str(t, {args[0].value()}, result);
        break;

    case PrimitiveValue::TFunction:
        result = "<function>";
        break;

    case PrimitiveValue::TVoid:
        result = "<void>";
        break;

    default:
        throw Error("Internal error: Invalid void type");
    }
}

void sqrt(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric>("sqrt", args);

    const float n = args[0].value().getNumAsFloat();
    if (n < 0.f) throw Error("Cannot take sqrt of negative value");
    result = std::sqrt(n);
}

void abs(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric>("abs", args);
    if (args[0].value().getType() == PrimitiveValue::TInteger)
        result = std::abs(args[0].value().getAsInt());
    else
        result = std::abs(args[0].value().getAsFloat());
}

void round(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric>("round", args);
    result = static_cast<long>(std::round(args[0].value().getNumAsFloat()));
}

void floor(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric>("floor", args);
    result = static_cast<long>(std::floor(args[0].value().getNumAsFloat()));
}

void ceil(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric>("ceil", args);
    result = static_cast<long>(std::ceil(args[0].value().getNumAsFloat()));
}

void sin(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric>("sin", args);
    result = static_cast<float>(std::sin(args[0].value().getNumAsFloat() / 180 * 3.1415926));
}

void cos(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric>("cos", args);
    result = static_cast<float>(std::cos(args[0].value().getNumAsFloat() / 180 * 3.1415926));
}

void tan(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric>("tan", args);
    result = static_cast<float>(std::tan(args[0].value().getNumAsFloat() / 180 * 3.1415926));
}

void atan2(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<TNumeric, TNumeric>("atan2", args);
    result = static_cast<float>(
        std::atan2(args[0].value().getNumAsFloat(), args[1].value().getNumAsFloat()) * 180 /
        3.1415926);
}

void min(SymbolTable&, const std::vector<Value>& args, Value& result) {
    PrimitiveValue mn(std::numeric_limits<long>::max());
    if (args.size() == 1) {
        const Value& arr = args[0].value();
        if (arr.value().getType() == PrimitiveValue::TArray) {
            for (const Value& v : arr.value().getAsArray()) {
                const Value& dr = v.value();
                if ((dr.value().getType() & TNumeric) == 0)
                    throw Error("Arguments to min(array) must all be Numeric");
                if (dr.value().getNumAsFloat() < mn.getNumAsFloat()) mn = dr.value();
            }
            result = mn;
            return;
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value dv = v.value();
        if ((dv.value().getType() & TNumeric) == 0)
            throw Error("Arguments to min() must all be Numeric");
        if (dv.value().getNumAsFloat() < mn.getNumAsFloat()) mn = dv.value();
    }
    result = mn;
}

void max(SymbolTable&, const std::vector<Value>& args, Value& result) {
    PrimitiveValue mx(std::numeric_limits<long>::min());
    if (args.size() == 1) {
        const Value& arr = args[0].value();
        if (arr.value().getType() == PrimitiveValue::TArray) {
            for (const Value& v : arr.value().getAsArray()) {
                const Value& dv = v.value();
                if ((dv.value().getType() & TNumeric) == 0)
                    throw Error("Arguments to max(array) must all be Numeric");
                if (dv.value().getNumAsFloat() > mx.getNumAsFloat()) mx = dv.value();
            }
            result = mx;
            return;
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value& dv = v.value();
        if ((dv.value().getType() & TNumeric) == 0)
            throw Error("Arguments to max() must all be numberic");
        if (dv.value().getNumAsFloat() > mx.getNumAsFloat()) mx = dv.value();
    }
    result = mx;
}

void sum(SymbolTable&, const std::vector<Value>& args, Value& result) {
    PrimitiveValue s(0);
    if (args.size() == 1) {
        const Value& arr = args[0].value();
        if (arr.value().getType() == PrimitiveValue::TArray) {
            for (const Value& v : arr.value().getAsArray()) {
                const Value& dv = v.value();
                if ((dv.value().getType() & TNumeric) == 0)
                    throw Error("Arguments to sum(array) must all be Numeric");
                if (s.getType() == PrimitiveValue::TInteger) {
                    if (dv.value().getType() == PrimitiveValue::TInteger) {
                        s = s.getAsInt() + dv.value().getAsInt();
                    }
                    else {
                        s = s.getNumAsFloat() + dv.value().getAsFloat();
                    }
                }
                else {
                    s = s.getAsFloat() + dv.value().getNumAsFloat();
                }
            }
            result = s;
            return;
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value& dv = v.value();
        if ((dv.value().getType() & TNumeric) == 0)
            throw Error("Arguments to sum() must all be numberic");
        if (s.getType() == PrimitiveValue::TInteger) {
            if (dv.value().getType() == PrimitiveValue::TInteger) {
                s = s.getAsInt() + dv.value().getAsInt();
            }
            else {
                s = s.getNumAsFloat() + dv.value().getAsFloat();
            }
        }
        else {
            s = s.getAsFloat() + dv.value().getNumAsFloat();
        }
    }
    result = s;
}

} // namespace

} // namespace script
} // namespace bl
