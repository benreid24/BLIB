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
namespace scripts
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

typedef std::pair<std::string, Function::CustomCB> builtin;
const std::vector<builtin> builtins = {
    builtin("print", print),       builtin("loginfo", loginfo), builtin("logdebug", logdebug),
    builtin("logerror", logerror), builtin("random", random),   builtin("sleep", sleep),
    builtin("time", time),         builtin("run", run),         builtin("exit", exit),
    builtin("error", error),       builtin("str", str),         builtin("num", num),
    builtin("sqrt", sqrt),         builtin("abs", abs),         builtin("round", round),
    builtin("floor", floor),       builtin("ceil", ceil),       builtin("sin", sin),
    builtin("cos", cos),           builtin("tan", tan),         builtin("atan2", atan2)};

} // namespace

void Library::addBuiltIns(SymbolTable& table) {
    for (const builtin& b : builtins) { table.set(b.first, Value(Function(b.second))); }
}

namespace
{
std::string argsToStr(SymbolTable& t, const std::vector<Value>& args) {
    std::stringstream ss;
    for (const Value& v : args) { ss << str(t, {v}).getAsString(); }
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
    if (args.size() != 2) throw Error("random() expects 2 arguments");
    if (args[0].getType() != Value::TNumeric || args[1].getType() != Value::TNumeric)
        throw Error("random() only takes Numeric arguments");
    const float mn = std::min(args[0].getAsNum(), args[1].getAsNum());
    const float mx = std::max(args[0].getAsNum(), args[1].getAsNum());
    return Value(util::Random::get(mn, mx));
}

Value sleep(SymbolTable& table, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("sleep() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("sleep() expects a Numeric time in ms");
    if (args[0].getAsNum() <= 0) throw Error("sleep() must be given a positive value");
    const unsigned int ms = args[0].getAsNum();
    unsigned int t        = 0;
    while (t < ms) {
        if (table.killed()) throw Error("Script killed");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ++t;
    }
    return Value();
}

Value time(SymbolTable&, const std::vector<Value>& args) {
    if (!args.empty()) throw Error("time() takes 0 arguments");
    return Value(util::Timer::get().timeElapsedRaw().asMilliseconds());
}

Value run(SymbolTable& table, const std::vector<Value>& args) {
    if (args.size() != 2) throw Error("run() takes 2 arguments");
    if (args[0].getType() != Value::TString || args[1].getType() != Value::TBool)
        throw Error("run() requires a String and a Bool");

    Script script(args[0].getAsString());
    if (!script.valid()) throw Error("Syntax error in script passed to run()");

    if (args[1].getAsBool()) {
        script.runBackground(table.manager());
        Value r;
        r.makeBool(true);
        return r;
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
    if (args.size() != 1) throw Error("error() takes 1 argument");
    if (args[0].getType() != Value::TString) throw Error("error() takes a String message");
    throw Error(args[0].getAsString());
}

Value str(SymbolTable& t, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("str() takes 1 argument");
    switch (args[0].getType()) {
    case Value::TBool:
        return Value(args[0].getAsBool() ? "true" : "false");
    case Value::TNumeric: {
        int v = args[0].getAsNum();
        if (v == args[0].getAsNum()) return Value(std::to_string(v));
        return Value(std::to_string(args[0].getAsNum()));
    }
    case Value::TString:
        return Value(args[0].getAsString());
    case Value::TArray: {
        std::string result = "[";
        if (!args[0].getAsArray().empty())
            result += str(t, {*(args[0].getAsArray()[0])}).getAsString();
        for (unsigned int i = 1; i < args[0].getAsArray().size(); ++i) {
            result += ", " + str(t, {*(args[0].getAsArray()[i])}).getAsString();
        }
        result += "]";
        return Value(result);
    }
    case Value::TRef: {
        Value::CPtr r = args[0].getAsRef().lock();
        if (!r) return Value("<expired reference>");
        return str(t, {*r});
    }
    case Value::TFunction:
        return Value("<function>");

    case Value::TVoid:
        return Value("<void>");

    default:
        throw Error("Internal error: Invalid Value type");
    }
}

Value num(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("num() takes 1 argument");
    if (args[0].getType() != Value::TString) throw Error("num() only takes String types");
    std::stringstream ss(args[0].getAsString());
    float f;
    ss >> f;
    return Value(f);
}

Value sqrt(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("sqrt() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("sqrt() only takes Numeric types");
    if (args[0].getAsNum() < 0) throw Error("Cannot take sqrt of negative value");
    return Value(std::sqrt(args[0].getAsNum()));
}

Value abs(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("abs() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("abs() only takes Numeric types");
    return Value(std::abs(args[0].getAsNum()));
}

Value round(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("round() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("round() only takes Numeric types");
    return Value(std::round(args[0].getAsNum()));
}

Value floor(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("floor() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("floor() only takes Numeric types");
    return Value(std::floor(args[0].getAsNum()));
}

Value ceil(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("ceil() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("ceil() only takes Numeric types");
    return Value(std::ceil(args[0].getAsNum()));
}

Value sin(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("sin() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("sin() only takes Numeric types");
    return Value(std::sin(args[0].getAsNum() / 180 * 3.1415926));
}

Value cos(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("cos() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("cos() only takes Numeric types");
    return Value(std::cos(args[0].getAsNum() / 180 * 3.1415926));
}

Value tan(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 1) throw Error("tan() takes 1 argument");
    if (args[0].getType() != Value::TNumeric) throw Error("tan() only takes Numeric types");
    return Value(std::tan(args[0].getAsNum() / 180 * 3.1415926));
}

Value atan2(SymbolTable&, const std::vector<Value>& args) {
    if (args.size() != 2) throw Error("atan2() takes 2 arguments");
    if (args[0].getType() != Value::TNumeric || args[1].getType() != Value::TNumeric)
        throw Error("atan2() only takes Numeric types");
    return Value(std::atan2(args[0].getAsNum(), args[1].getAsNum()) * 180 / 3.1415926);
}

} // namespace

} // namespace scripts
} // namespace bl