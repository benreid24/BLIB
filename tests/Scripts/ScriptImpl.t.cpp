#include <Scripts/ScriptImpl.hpp>

#include <Scripts/Parser.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
namespace unittest
{
using G = Parser::Grammar;

TEST(ScriptImpl, ValueTree) {
    parser::Grammar grammar     = Parser::getGrammar();
    parser::Tokenizer tokenizer = Parser::getTokenizer();
    grammar.setStart(G::Value);
    bl::parser::Parser parser(grammar, tokenizer);
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
SymbolTable genVar(const std::string& name, const Value& value) {
    SymbolTable t;
    t.set(name, value);
    return t;
}

SymbolTable genRef(const std::string& name, const Value& value, const std::string& refName) {
    SymbolTable t = genVar(name, value);
    Value ref     = t.get(name);
    t.set(refName, ref);
    return t;
}

SymbolTable genProp(const std::string name, const std::string& propName, const Value& value,
                    const Value& propVal) {
    SymbolTable t     = genVar(name, value);
    ReferenceValue rv = t.get(name);
    rv.deref().setProperty(propName, propVal);
    return t;
}
} // namespace

TEST_P(ScriptImplValueTest, ValueTest) {
    parser::Grammar grammar     = Parser::getGrammar();
    parser::Tokenizer tokenizer = Parser::getTokenizer();
    grammar.setStart(G::Value);
    bl::parser::Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());

    ValueTest td           = GetParam();
    parser::Node::Ptr root = parser.parse(td.input);
    ASSERT_NE(root.get(), nullptr);
    ASSERT_EQ(root->type, G::Value);

    Value actual;
    try {
        actual = ScriptImpl::computeValue(root, td.symbols);
    } catch (const Error& error) { FAIL() << error.message(); }

    ASSERT_EQ(td.result.value().getType(), actual.value().getType());
    switch (td.result.value().getType()) {
    case PrimitiveValue::TBool:
        EXPECT_EQ(td.result.value().getAsBool(), actual.value().getAsBool()) << td.input;
        break;
    case PrimitiveValue::TNumeric:
        EXPECT_EQ(td.result.value().getAsNum(), actual.value().getAsNum()) << td.input;
        break;
    case PrimitiveValue::TString:
        EXPECT_EQ(td.result.value().getAsString(), actual.value().getAsString()) << td.input;
        break;
    case PrimitiveValue::TArray: {
        ASSERT_EQ(td.result.value().getAsArray().size(), actual.value().getAsArray().size())
            << td.input;
        for (unsigned int i = 0; i < td.result.value().getAsArray().size(); ++i) {
            EXPECT_EQ(td.result.value().getAsArray()[i].value().getAsNum(),
                      actual.value().getAsArray()[i].value().getAsNum())
                << td.input;
        }
        break;
    }
    case PrimitiveValue::TFunction:
        EXPECT_EQ(td.result.value().getAsFunction(), actual.value().getAsFunction()) << td.input;
        break;
    default:
        FAIL();
    }
}

INSTANTIATE_TEST_SUITE_P(
    ScriptImplValues, ScriptImplValueTest,
    ::testing::Values(
        ValueTest("3+5*2+4^2-6/3+5*2^(1+1)", Value(47.f), SymbolTable()),
        ValueTest("5+6-3", Value(8.f), SymbolTable()),
        ValueTest("5-6+3", Value(2.f), SymbolTable()),
        ValueTest("5+6/2-3", Value(5.f), SymbolTable()),
        ValueTest("true and false", Value(false), SymbolTable()),
        ValueTest("true or false", Value(true), SymbolTable()),
        ValueTest("false or true and false", Value(false), SymbolTable()),
        ValueTest("false or not true", Value(false), SymbolTable()),
        ValueTest("not false or not true", Value(true), SymbolTable()),
        ValueTest("true and not false", Value(true), SymbolTable()),
        ValueTest("false or not true and false", Value(false), SymbolTable()),
        ValueTest("true and false or true", Value(true), SymbolTable()),
        ValueTest("5 < 6", Value(true), SymbolTable()),
        ValueTest("5<5", Value(false), SymbolTable()),
        ValueTest("5<=5", Value(true), SymbolTable()),
        ValueTest("5>6", Value(false), SymbolTable()),
        ValueTest("5>=5", Value(true), SymbolTable()),
        ValueTest("17 == 17", Value(true), SymbolTable()),
        ValueTest("5 != 6", Value(true), SymbolTable()),
        ValueTest("[3,4] + 5", Value(ArrayValue{Value(3.f), Value(4.f), Value(5.f)}),
                  SymbolTable()),
        ValueTest("\"cat\" * 3", Value("catcatcat"), SymbolTable()),
        ValueTest("[3] * 3", Value(ArrayValue{Value(3.f), Value(3.f), Value(3.f)}), SymbolTable()),
        ValueTest("\"cat\" + 5", Value("cat5"), SymbolTable()),
        ValueTest("5 == var", Value(true), genVar("var", Value(5.f))),
        ValueTest("[\"cat\", \"dog\"] == arr", Value(true),
                  genVar("arr", Value(ArrayValue({Value("cat"), Value("dog")})))),
        ValueTest("var == ref", Value(true), genRef("var", Value(7.f), "ref")),
        ValueTest("var.prop == 5", Value(true), genProp("var", "prop", Value(), Value(5.f)))));

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
    ASSERT_EQ(t.result.value().getType(), PrimitiveValue::TNumeric)
        << "Test only works with Numeric type";

    parser::Grammar grammar     = Parser::getGrammar();
    parser::Tokenizer tokenizer = Parser::getTokenizer();
    grammar.setStart(G::Value);
    bl::parser::Parser cparser(grammar, tokenizer);
    ASSERT_TRUE(cparser.valid());
    grammar.setStart(G::Statement);
    bl::parser::Parser fparser(grammar, tokenizer);
    ASSERT_TRUE(fparser.valid());

    parser::Node::Ptr fdef = fparser.parse(t.fdef);
    ASSERT_NE(fdef.get(), nullptr);
    parser::Node::Ptr call = cparser.parse(t.call);
    ASSERT_NE(call.get(), nullptr);

    try {
        SymbolTable table;
        ScriptImpl::runStatement(fdef, table);
        const Value v = ScriptImpl::computeValue(call, table);
        ASSERT_EQ(v.value().getType(), PrimitiveValue::TNumeric);
        ASSERT_EQ(v.value().getAsNum(), t.result.value().getAsNum());
    } catch (const Error& err) {
        FAIL() << err.stacktrace() << "\n" << err.message() << "\n" << t.fdef << "\n" << t.call;
    }
}

const std::string eliftest =
    "def func(a, b, c) { if (a < b) return a; elif (b < c) return b; else return c;}";
const std::string whiletest = "def func(a, l) { i = 0; sum = 0; while (i < l) { sum = sum + "
                              "a; i = i + 1; } return sum;}";

INSTANTIATE_TEST_SUITE_P(
    ScriptImplFunctions, ScriptImplFunctionTest,
    ::testing::Values(
        FunctionTest("def func(var) { return var; }", "func(5)", 5.f),
        FunctionTest("def func(l,r) { return l-r; }", "func(5, 4)", 1.f),
        FunctionTest("def func(l,r) { s = l+r; return s-5; }", "func(5, 4)", 4.f),
        FunctionTest("def func(l,r) { s = [l,r]; return s.length; }", "func(5, 4)", 2.f),
        FunctionTest("def func(l,r) { s = [l,r]; s.append(l,r); return s.length; }", "func(5, 4)",
                     4.f),
        FunctionTest("def func(l,r) { s = [l,r]; s.insert(0, l); return s[0]; }", "func(5, 4)",
                     5.f),
        FunctionTest("def func(l,r) { s = [l,r]; s.resize(3, r); return s.length+s[2]; }",
                     "func(5, 4)", 7.f),
        FunctionTest("def func(l,r) { s = [l,r]; s.erase(0); return s[0]; }", "func(5, 4)", 4.f),
        FunctionTest("def func(l,r) { s = [l,r]; s.clear(); return s.length; }", "func(5, 4)", 0.f),
        FunctionTest(eliftest, "func(1,2,3)", 1.f), FunctionTest(eliftest, "func(5,2,3)", 2.f),
        FunctionTest(eliftest, "func(5,4,3)", 3.f), FunctionTest(whiletest, "func(5, 2)", 10.f),
        FunctionTest(whiletest, "func(5, 0)", 0.f),
        FunctionTest("def f() { a = [1,2,3]; a[2] = 5; return a[2];}", "f()", 5.f),
        FunctionTest("def f() { v = 5; v.p = 2; v.p.p = 5; return v.p.p;}", "f()", 5.f),
        FunctionTest("def f() { v = 5; r = &v; v = 6; return r; }", "f()", 6.f),
        FunctionTest("def f() { v = 5; r = &v; r = 6; return v; }", "f()", 6.f),
        FunctionTest("def f() { v = 5; r = &v; n = &r; v = 6; return n; }", "f()", 6.f),
        FunctionTest("def f(a) { s = 0; for (x in a) s = s + x; return s; }", "f([1,2,3])", 6.f),
        FunctionTest("def f(x) { x.a = 5; x.b = 10; s = 0; for (k in x.keys()) { s = s + "
                     "x.at(k); } return s; }",
                     "f(5)", 15.f),
        FunctionTest("def f() { u.a = 5; return u.a; }", "f()", 5.f)
        //
        ));

} // namespace unittest
} // namespace script
} // namespace bl
