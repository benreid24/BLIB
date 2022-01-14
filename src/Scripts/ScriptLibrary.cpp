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
void print(SymbolTable&, const std::vector<Value>& args, Value& result);
void loginfo(SymbolTable&, const std::vector<Value>& args, Value& result);
void logerror(SymbolTable&, const std::vector<Value>& args, Value& result);
void logdebug(SymbolTable&, const std::vector<Value>& args, Value& result);
void random(SymbolTable&, const std::vector<Value>& args, Value& result);
void sleep(SymbolTable&, const std::vector<Value>& args, Value& result);
void time(SymbolTable&, const std::vector<Value>& args, Value& result);
void run(SymbolTable&, const std::vector<Value>& args, Value& result);
void exit(SymbolTable&, const std::vector<Value>& args, Value& result);
void error(SymbolTable&, const std::vector<Value>& args, Value& result);
void str(SymbolTable&, const std::vector<Value>& args, Value& result);
void num(SymbolTable&, const std::vector<Value>& args, Value& result);
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

typedef std::pair<std::string, Function::CustomCB> builtin;
const std::vector<builtin> builtins = {
    builtin("print", print),       builtin("loginfo", loginfo), builtin("logdebug", logdebug),
    builtin("logerror", logerror), builtin("random", random),   builtin("sleep", sleep),
    builtin("time", time),         builtin("run", run),         builtin("exit", exit),
    builtin("error", error),       builtin("str", str),         builtin("num", num),
    builtin("sqrt", sqrt),         builtin("abs", abs),         builtin("round", round),
    builtin("floor", floor),       builtin("ceil", ceil),       builtin("sin", sin),
    builtin("cos", cos),           builtin("tan", tan),         builtin("atan2", atan2),
    builtin("min", min),           builtin("max", max),         builtin("sum", sum)};

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

void random(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric, PrimitiveValue::TNumeric>("random", args);

    const float l = args[0].value().deref().getAsNum();
    const float r = args[1].value().deref().getAsNum();
    result        = util::Random::get(std::min(l, r), std::max(l, r));
}

void sleep(SymbolTable& table, const std::vector<Value>& args, Value&) {
    Value::validateArgs<PrimitiveValue::TNumeric>("sleep", args);

    if (args[0].value().deref().getAsNum() <= 0)
        throw Error("sleep() must be given a positive value");
    const unsigned int ms = args[0].value().deref().getAsNum();
    table.waitFor(ms);
    if (table.killed()) throw Error("Script killed");
}

void time(SymbolTable&, const std::vector<Value>& args, Value& result) {
    if (!args.empty()) throw Error("time() takes 0 arguments");
    result = static_cast<float>(util::Timer::get().timeElapsedRaw().asMilliseconds());
}

void run(SymbolTable& table, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TString, PrimitiveValue::TBool>("run", args);

    Script script(args[0].value().deref().getAsString(), table.base());
    if (!script.valid()) throw Error("Syntax error in script passed to run()");

    if (args[1].value().deref().getAsBool()) {
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
    throw Error(args[0].value().deref().getAsString());
}

void str(SymbolTable& t, const std::vector<Value>& args, Value& result) {
    if (args.size() != 1) throw Error("str() takes 1 argument");

    switch (args[0].value().getType()) {
    case PrimitiveValue::TBool:
        result = (args[0].value().getAsBool() ? "true" : "false");
        break;
    case PrimitiveValue::TNumeric: {
        const long int v = args[0].value().getAsNum();
        if (v == args[0].value().getAsNum()) { result = std::to_string(v); }
        else {
            result = std::to_string(args[0].value().getAsNum());
        }
        break;
    }
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
        str(t, {args[0].value().deref()}, result);
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

void num(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TString>("num", args);
    result = std::atof(args[0].value().deref().getAsString().c_str());
}

void sqrt(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric>("sqrt", args);

    const float n = args[0].value().deref().getAsNum();
    if (n < 0.f) throw Error("Cannot take sqrt of negative value");
    result = std::sqrt(n);
}

void abs(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric>("abs", args);
    result = std::abs(args[0].value().deref().getAsNum());
}

void round(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric>("round", args);
    result = std::round(args[0].value().deref().getAsNum());
}

void floor(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric>("floor", args);
    result = std::floor(args[0].value().deref().getAsNum());
}

void ceil(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric>("ceil", args);
    result = std::ceil(args[0].value().deref().getAsNum());
}

void sin(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric>("sin", args);
    result = static_cast<float>(std::sin(args[0].value().deref().getAsNum() / 180 * 3.1415926));
}

void cos(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric>("cos", args);
    result = static_cast<float>(std::cos(args[0].value().deref().getAsNum() / 180 * 3.1415926));
}

void tan(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric>("tan", args);
    result = static_cast<float>(std::tan(args[0].value().deref().getAsNum() / 180 * 3.1415926));
}

void atan2(SymbolTable&, const std::vector<Value>& args, Value& result) {
    Value::validateArgs<PrimitiveValue::TNumeric, PrimitiveValue::TNumeric>("atan2", args);
    result = static_cast<float>(
        std::atan2(args[0].value().deref().getAsNum(), args[1].value().getAsNum()) * 180 /
        3.1415926);
}

void min(SymbolTable&, const std::vector<Value>& args, Value& result) {
    float mn = 1000000000000.f;
    if (args.size() == 1) {
        const Value& arr = args[0].value().deref();
        if (arr.value().getType() == PrimitiveValue::TArray) {
            for (const Value& v : arr.value().getAsArray()) {
                const Value& dr = v.value().deref();
                if (dr.value().getType() != PrimitiveValue::TNumeric)
                    throw Error("Arguments to min(array) must all be Numeric");
                if (dr.value().getAsNum() < mn) mn = dr.value().getAsNum();
            }
            result = mn;
            return;
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value dv = v.value().deref();
        if (dv.value().getType() != PrimitiveValue::TNumeric)
            throw Error("Arguments to min() must all be numberic");
        if (dv.value().getAsNum() < mn) mn = dv.value().getAsNum();
    }
    result = mn;
}

void max(SymbolTable&, const std::vector<Value>& args, Value& result) {
    float mx = -1000000000000.f;
    if (args.size() == 1) {
        const Value& arr = args[0].value().deref();
        if (arr.value().getType() == PrimitiveValue::TArray) {
            for (const Value& v : arr.value().getAsArray()) {
                const Value& dv = v.value().deref();
                if (dv.value().getType() != PrimitiveValue::TNumeric)
                    throw Error("Arguments to max(array) must all be Numeric");
                if (dv.value().getAsNum() > mx) mx = dv.value().getAsNum();
            }
            result = mx;
            return;
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value& dv = v.value().deref();
        if (dv.value().getType() != PrimitiveValue::TNumeric)
            throw Error("Arguments to max() must all be numberic");
        if (dv.value().getAsNum() > mx) mx = dv.value().getAsNum();
    }
    result = mx;
}

void sum(SymbolTable&, const std::vector<Value>& args, Value& result) {
    float s = 0.f;
    if (args.size() == 1) {
        const Value& arr = args[0].value().deref();
        if (arr.value().getType() == PrimitiveValue::TArray) {
            for (const Value& v : arr.value().getAsArray()) {
                const Value& dv = v.value().deref();
                if (dv.value().getType() != PrimitiveValue::TNumeric)
                    throw Error("Arguments to sum(array) must all be Numeric");
                s += dv.value().getAsNum();
            }
            result = s;
            return;
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value& dv = v.value().deref();
        if (dv.value().getType() != PrimitiveValue::TNumeric)
            throw Error("Arguments to sum() must all be numberic");
        s += dv.value().getAsNum();
    }
    result = s;
}

} // namespace

} // namespace script
} // namespace bl
