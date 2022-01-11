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
Value print(SymbolTable&, const std::vector<Value>& args);
Value loginfo(SymbolTable&, const std::vector<Value>& args);
Value logerror(SymbolTable&, const std::vector<Value>& args);
Value logdebug(SymbolTable&, const std::vector<Value>& args);
Value random(SymbolTable&, const std::vector<Value>& args);
Value sleep(SymbolTable&, const std::vector<Value>& args);
Value time(SymbolTable&, const std::vector<Value>& args);
Value run(SymbolTable&, const std::vector<Value>& args);
Value exit(SymbolTable&, const std::vector<Value>& args);
Value error(SymbolTable&, const std::vector<Value>& args);
Value str(SymbolTable&, const std::vector<Value>& args);
Value num(SymbolTable&, const std::vector<Value>& args);
Value sqrt(SymbolTable&, const std::vector<Value>& args);
Value abs(SymbolTable&, const std::vector<Value>& args);
Value round(SymbolTable&, const std::vector<Value>& args);
Value floor(SymbolTable&, const std::vector<Value>& args);
Value ceil(SymbolTable&, const std::vector<Value>& args);
Value sin(SymbolTable&, const std::vector<Value>& args);
Value cos(SymbolTable&, const std::vector<Value>& args);
Value tan(SymbolTable&, const std::vector<Value>& args);
Value atan2(SymbolTable&, const std::vector<Value>& args);
Value min(SymbolTable&, const std::vector<Value>& args);
Value max(SymbolTable&, const std::vector<Value>& args);
Value sum(SymbolTable&, const std::vector<Value>& args);

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
    for (const Value& v : args) { ss << str(t, {v}).value().getAsString(); }
    return ss.str();
}

Value print(SymbolTable& t, const std::vector<Value>& args) {
    std::cout << argsToStr(t, args) << std::endl;
    return Value();
}

Value loginfo(SymbolTable& t, const std::vector<Value>& args) {
    bl::logging::Logger::info() << argsToStr(t, args);
    return Value();
}

Value logerror(SymbolTable& t, const std::vector<Value>& args) {
    bl::logging::Logger::error() << argsToStr(t, args);
    return Value();
}

Value logdebug(SymbolTable& t, const std::vector<Value>& args) {
    bl::logging::Logger::debug() << argsToStr(t, args);
    return Value();
}

Value random(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric, PrimitiveValue::TNumeric>("random", args);

    const float l = args[0].value().deref().getAsNum();
    const float r = args[1].value().deref().getAsNum();
    return Value(util::Random::get(std::min(l, r), std::max(l, r)));
}

Value sleep(SymbolTable& table, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("sleep", args);

    if (args[0].value().deref().getAsNum() <= 0)
        throw Error("sleep() must be given a positive value");
    const unsigned int ms = args[0].value().deref().getAsNum();
    table.waitFor(ms);
    if (table.killed()) throw Error("Script killed");
    return Value();
}

Value time(SymbolTable&, const std::vector<Value>& args) {
    if (!args.empty()) throw Error("time() takes 0 arguments");
    return Value(static_cast<float>(util::Timer::get().timeElapsedRaw().asMilliseconds()));
}

Value run(SymbolTable& table, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TString, PrimitiveValue::TBool>("run", args);

    Script script(args[0].value().deref().getAsString(), table.base());
    if (!script.valid()) throw Error("Syntax error in script passed to run()");

    if (args[1].value().deref().getAsBool()) {
        script.runBackground(table.manager());
        return Value(true);
    }
    else {
        const std::optional<Value> r = script.run(table.manager());
        return r.value_or(Value());
    }
}

Value exit(SymbolTable&, const std::vector<Value>& args) {
    if (!args.empty()) throw Error("exit() takes 0 arguments");
    throw Exit();
}

Value error(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TString>("error", args);
    throw Error(args[0].value().deref().getAsString());
}

Value str(SymbolTable& t, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("str() takes 1 argument");

    switch (args[0].value().getType()) {
    case PrimitiveValue::TBool:
        return Value(args[0].value().getAsBool() ? "true" : "false");
    case PrimitiveValue::TNumeric: {
        const int v = args[0].value().getAsNum();
        if (v == args[0].value().getAsNum()) return Value(std::to_string(v));
        return Value(std::to_string(args[0].value().getAsNum()));
    }
    case PrimitiveValue::TString:
        return Value(args[0].value().getAsString());
    case PrimitiveValue::TArray: {
        std::string result = "[";
        if (!args[0].value().getAsArray().empty())
            result += str(t, {args[0].value().getAsArray()[0]}).value().getAsString();
        for (unsigned int i = 1; i < args[0].value().getAsArray().size(); ++i) {
            result += ", " + str(t, {args[0].value().getAsArray()[i]}).value().getAsString();
        }
        result += "]";
        return Value(result);
    }
    case PrimitiveValue::TRef:
        return str(t, {args[0].value().deref()});

    case PrimitiveValue::TFunction:
        return Value("<function>");

    case PrimitiveValue::TVoid:
        return Value("<void>");

    default:
        throw Error("Internal error: Invalid Value type");
    }
}

Value num(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TString>("num", args);
    return Value(std::atof(args[0].value().deref().getAsString().c_str()));
}

Value sqrt(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("sqrt", args);

    const float n = args[0].value().deref().getAsNum();
    if (n < 0.f) throw Error("Cannot take sqrt of negative value");
    return Value(std::sqrt(n));
}

Value abs(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("abs", args);
    return Value(std::abs(args[0].value().deref().getAsNum()));
}

Value round(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("round", args);
    return Value(std::round(args[0].value().deref().getAsNum()));
}

Value floor(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("floor", args);
    return Value(std::floor(args[0].value().deref().getAsNum()));
}

Value ceil(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("ceil", args);
    return Value(std::ceil(args[0].value().deref().getAsNum()));
}

Value sin(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("sin", args);
    return Value(
        static_cast<float>(std::sin(args[0].value().deref().getAsNum() / 180 * 3.1415926)));
}

Value cos(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("cos", args);
    return Value(
        static_cast<float>(std::cos(args[0].value().deref().getAsNum() / 180 * 3.1415926)));
}

Value tan(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric>("tan", args);
    return Value(
        static_cast<float>(std::tan(args[0].value().deref().getAsNum() / 180 * 3.1415926)));
}

Value atan2(SymbolTable&, const std::vector<Value>& args) {
    Value::validateArgs<PrimitiveValue::TNumeric, PrimitiveValue::TNumeric>("atan2", args);
    return Value(static_cast<float>(
        std::atan2(args[0].value().deref().getAsNum(), args[1].value().getAsNum()) * 180 /
        3.1415926));
}

Value min(SymbolTable&, const std::vector<Value>& args) {
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
            return Value(mn);
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value dv = v.value().deref();
        if (dv.value().getType() != PrimitiveValue::TNumeric)
            throw Error("Arguments to min() must all be numberic");
        if (dv.value().getAsNum() < mn) mn = dv.value().getAsNum();
    }
    return Value(mn);
}

Value max(SymbolTable&, const std::vector<Value>& args) {
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
            return Value(mx);
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value& dv = v.value().deref();
        if (dv.value().getType() != PrimitiveValue::TNumeric)
            throw Error("Arguments to max() must all be numberic");
        if (dv.value().getAsNum() > mx) mx = dv.value().getAsNum();
    }
    return Value(mx);
}

Value sum(SymbolTable&, const std::vector<Value>& args) {
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
            return Value(s);
        }
    }
    // deliberate fallthrough
    for (const Value& v : args) {
        const Value& dv = v.value().deref();
        if (dv.value().getType() != PrimitiveValue::TNumeric)
            throw Error("Arguments to sum() must all be numberic");
        s += dv.value().getAsNum();
    }
    return Value(s);
}

} // namespace

} // namespace script
} // namespace bl
