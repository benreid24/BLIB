#include <Scripts/ScriptImpl.hpp>

#include <Scripts/Parser.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace scripts
{
namespace unittest
{
using G = Parser::Grammar;

TEST(ScriptImpl, ValueTree) {
    parser::Grammar grammar     = Parser::getGrammar();
    parser::Tokenizer tokenizer = Parser::getTokenizer();
    grammar.setStart(G::Value);
    bl::Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());

    const std::string script = "5 + 6^(7*12-2*dog) >= wogh";
    parser::Node::Ptr root   = parser.parse(script);
    ASSERT_NE(root.get(), nullptr);
    ASSERT_EQ(root->type, G::Value);
}

struct ValueTest {
    const std::string input;
    const Value result;
    SymbolTable symbols;

    ValueTest(const std::string& input, const Value& result, const SymbolTable& symbols)
    : input(input)
    , result(result)
    , symbols(symbols) {}
};

class ScriptImplValueTest : public ::testing::TestWithParam<ValueTest> {};

namespace
{
Value boolValue(bool val) {
    Value v;
    v.makeBool(val);
    return v;
}

SymbolTable genVar(const std::string& name, const Value& value) {
    SymbolTable t;
    t.set(name, value);
    return t;
}

SymbolTable genRef(const std::string& name, const Value& value, const std::string& refName) {
    SymbolTable t = genVar(name, value);
    Value ref     = Value::Ref(t.get(name));
    t.set(refName, ref);
    return t;
}

SymbolTable genArray(const std::string& name, const Value& fill, unsigned int len) {
    SymbolTable t;
    Value::Array a;
    Value::Ptr f(new Value(fill));
    a.resize(len, f);
    t.set(name, a);
    return t;
}

SymbolTable genProp(const std::string name, const std::string& propName, const Value& value,
                    const Value& propVal) {
    SymbolTable t = genVar(name, value);
    Value::Ptr v  = t.get(name);
    v->setProperty(propName, propVal);
    return t;
}
} // namespace

TEST_P(ScriptImplValueTest, ValueTest) {
    parser::Grammar grammar     = Parser::getGrammar();
    parser::Tokenizer tokenizer = Parser::getTokenizer();
    grammar.setStart(G::Value);
    bl::Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());

    ValueTest td           = GetParam();
    parser::Node::Ptr root = parser.parse(td.input);
    ASSERT_NE(root.get(), nullptr);
    ASSERT_EQ(root->type, G::Value);

    Value actual;
    try {
        actual = ScriptImpl::computeValue(root, td.symbols);
    } catch (const Error& error) { FAIL() << error.message(); }

    ASSERT_EQ(td.result.getType(), actual.getType());
    switch (td.result.getType()) {
    case Value::TBool:
        EXPECT_EQ(td.result.getAsBool(), actual.getAsBool()) << td.input;
        break;
    case Value::TNumeric:
        EXPECT_EQ(td.result.getAsNum(), actual.getAsNum()) << td.input;
        break;
    case Value::TString:
        EXPECT_EQ(td.result.getAsString(), actual.getAsString()) << td.input;
        break;
    case Value::TArray: {
        ASSERT_EQ(td.result.getAsArray().size(), actual.getAsArray().size()) << td.input;
        for (unsigned int i = 0; i < td.result.getAsArray().size(); ++i) {
            EXPECT_EQ(td.result.getAsArray()[i]->getAsNum(),
                      actual.getAsArray()[i]->getAsNum())
                << td.input;
        }
        break;
    }
    case Value::TFunction:
        EXPECT_EQ(td.result.getAsFunction(), actual.getAsFunction()) << td.input;
        break;
    default:
        FAIL();
    }
}

INSTANTIATE_TEST_SUITE_P(
    ScriptImplValues, ScriptImplValueTest,
    ::testing::Values(ValueTest("3+5*2+4^2-6/3+5*2^(1+1)", Value(47), SymbolTable()),
                      ValueTest("5+6-3", Value(8), SymbolTable()),
                      ValueTest("5-6+3", Value(2), SymbolTable()),
                      ValueTest("5+6/2-3", Value(5), SymbolTable()),
                      ValueTest("true and false", boolValue(false), SymbolTable()),
                      ValueTest("true or false", boolValue(true), SymbolTable()),
                      ValueTest("false or true and false", boolValue(false), SymbolTable()),
                      ValueTest("false or not true", boolValue(false), SymbolTable()),
                      ValueTest("not false or not true", boolValue(true), SymbolTable()),
                      ValueTest("true and not false", boolValue(true), SymbolTable()),
                      ValueTest("false or not true and false", boolValue(false),
                                SymbolTable()),
                      ValueTest("true and false or true", boolValue(true), SymbolTable()),
                      ValueTest("5 < 6", boolValue(true), SymbolTable()),
                      ValueTest("5<5", boolValue(false), SymbolTable()),
                      ValueTest("5<=5", boolValue(true), SymbolTable()),
                      ValueTest("5>6", boolValue(false), SymbolTable()),
                      ValueTest("5>=5", boolValue(true), SymbolTable()),
                      ValueTest("17 == 17", boolValue(true), SymbolTable()),
                      ValueTest("5 != 6", boolValue(true), SymbolTable()),
                      ValueTest("5 == var", boolValue(true), genVar("var", Value(5))),
                      ValueTest("[\"cat\", \"dog\"] == arr", boolValue(true),
                                genVar("arr", Value({Value("cat"), Value("dog")}))),
                      ValueTest("var == ref", boolValue(true), genRef("var", Value(7), "ref")),
                      ValueTest("var.prop == 5", boolValue(true),
                                genProp("var", "prop", Value(), Value(5)))));

struct FunctionTest {
    const std::string fdef;
    const std::string call;
    const Value result;

    FunctionTest(const std::string& fdef, const std::string& call, const Value& result)
    : fdef(fdef)
    , call(call)
    , result(result) {}
};

class ScriptImplFunctionTest : public ::testing::TestWithParam<FunctionTest> {};

TEST_P(ScriptImplFunctionTest, FunctionTest) {
    const FunctionTest t = GetParam();
    ASSERT_EQ(t.result.getType(), Value::TNumeric) << "Test only works with Numeric type";

    parser::Grammar grammar     = Parser::getGrammar();
    parser::Tokenizer tokenizer = Parser::getTokenizer();
    grammar.setStart(G::Value);
    bl::Parser cparser(grammar, tokenizer);
    ASSERT_TRUE(cparser.valid());
    grammar.setStart(G::Statement);
    bl::Parser fparser(grammar, tokenizer);
    ASSERT_TRUE(fparser.valid());

    parser::Node::Ptr fdef = fparser.parse(t.fdef);
    ASSERT_NE(fdef.get(), nullptr);
    parser::Node::Ptr call = cparser.parse(t.call);
    ASSERT_NE(call.get(), nullptr);

    try {
        SymbolTable table;
        ScriptImpl::runStatement(fdef, table);
        const Value v = ScriptImpl::computeValue(call, table);
        ASSERT_EQ(v.getType(), Value::TNumeric);
        ASSERT_EQ(v.getAsNum(), t.result.getAsNum());
    } catch (const Error& err) {
        FAIL() << err.stacktrace() << "\n"
               << err.message() << "\n"
               << t.fdef << "\n"
               << t.call;
    }
}

const std::string eliftest =
    "def func(a, b, c) { if (a < b) return a; elif (b < c) return b; else return c;}";
const std::string whiletest = "def func(a, l) { i = 0; sum = 0; while (i < l) { sum = sum + "
                              "a; i = i + 1; } return sum;}";

INSTANTIATE_TEST_CASE_P(
    ScriptImplFunctions, ScriptImplFunctionTest,
    ::testing::Values(
        FunctionTest("def func(var) { return var; }", "func(5)", 5),
        FunctionTest("def func(l,r) { return l-r; }", "func(5, 4)", 1),
        FunctionTest("def func(l,r) { s = l+r; return s-5; }", "func(5, 4)", 4),
        FunctionTest("def func(l,r) { s = [l,r]; return s.length; }", "func(5, 4)", 2),
        FunctionTest("def func(l,r) { s = [l,r]; s.append(l,r); return s.length; }",
                     "func(5, 4)", 4),
        FunctionTest("def func(l,r) { s = [l,r]; s.insert(0, l); return s[0]; }", "func(5, 4)",
                     5),
        FunctionTest("def func(l,r) { s = [l,r]; s.resize(3, r); return s.length+s[2]; }",
                     "func(5, 4)", 7),
        FunctionTest("def func(l,r) { s = [l,r]; s.erase(0); return s[0]; }", "func(5, 4)", 4),
        FunctionTest("def func(l,r) { s = [l,r]; s.clear(); return s.length; }", "func(5, 4)",
                     0),
        FunctionTest(eliftest, "func(1,2,3)", 1), FunctionTest(eliftest, "func(5,2,3)", 2),
        FunctionTest(eliftest, "func(5,4,3)", 3), FunctionTest(whiletest, "func(5, 2)", 10),
        FunctionTest(whiletest, "func(5, 0)", 0),
        FunctionTest("def f() { a = [1,2,3]; a[2] = 5; return a[2];}", "f()", 5),
        FunctionTest("def f() { v = 5; v.p = 2; v.p.p = 5; return v.p.p;}", "f()", 5),
        FunctionTest("def f() { v = 5; r = &v; v = 6; return r; }", "f()", 6),
        FunctionTest("def f() { v = 5; r = &v; r = 6; return v; }", "f()", 6),
        FunctionTest("def f() { v = 5; r = &v; n = &r; v = 6; return n; }", "f()", 6)
        //
        ));

} // namespace unittest
} // namespace scripts
} // namespace bl