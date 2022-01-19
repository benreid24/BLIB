#include <BLIB/Scripts/SymbolTable.hpp>
#include <BLIB/Scripts/Value.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
namespace unittest
{
TEST(ScriptValue, PrimitiveEmplace) {
    const Value bval(true);
    ASSERT_EQ(bval.value().getType(), PrimitiveValue::TBool);
    EXPECT_EQ(bval.value().getAsBool(), true);

    const Value nval(56.f);
    ASSERT_EQ(nval.value().getType(), PrimitiveValue::TFloat);
    EXPECT_EQ(nval.value().getAsFloat(), 56.f);

    const Value ival(-56);
    ASSERT_EQ(ival.value().getType(), PrimitiveValue::TInteger);
    EXPECT_EQ(ival.value().getAsInt(), -56);

    Value sval("str");
    ASSERT_EQ(sval.value().getType(), PrimitiveValue::TString);
    EXPECT_EQ(sval.value().getAsString(), "str");

    const Value aval(ArrayValue{bval, nval, sval});
    ASSERT_EQ(aval.value().getType(), PrimitiveValue::TArray);
    const auto& arr = aval.value().getAsArray();
    ASSERT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0].value().getType(), PrimitiveValue::TBool);
    EXPECT_EQ(arr[1].value().getType(), PrimitiveValue::TFloat);
    EXPECT_EQ(arr[2].value().getType(), PrimitiveValue::TString);

    const Value rval(ReferenceValue{&sval});
    ASSERT_EQ(rval.value().getType(), PrimitiveValue::TString);
    ASSERT_EQ(&rval.value(), &sval.value());
}

TEST(ScriptValue, GeneralProps) {
    const Value bval(true);
    const Value nval(56.f);
    const Value sval("str");

    Value value;
    value.setProperty("bval", bval);
    value.setProperty("nval", nval);
    value.setProperty("sval", sval);

    const auto& bref = value.getProperty("bval", false).deref().value();
    ASSERT_EQ(bref.getType(), PrimitiveValue::TBool);
    EXPECT_EQ(bref.getAsBool(), true);

    const auto& nref = value.getProperty("nval", false).deref().value();
    ASSERT_EQ(nref.getType(), PrimitiveValue::TFloat);
    EXPECT_EQ(nref.getAsFloat(), 56.f);

    const auto& sref = value.getProperty("sval", false).deref().value();
    ASSERT_EQ(sref.getType(), PrimitiveValue::TString);
    EXPECT_EQ(sref.getAsString(), "str");

    EXPECT_THROW(value.getProperty("dne", false), Error);

    const auto& vref = value.getProperty("create", true).deref().value();
    EXPECT_EQ(vref.getType(), PrimitiveValue::TVoid);
}

TEST(ScriptValue, Builtins) {
    SymbolTable table;
    Value aval(ArrayValue{Value(1.f), Value(2.f), Value(3.f)});
    Value ret;

    // length
    ReferenceValue len = aval.getProperty("length", false);
    const auto& lref   = len.deref().value();
    ASSERT_EQ(lref.getType(), PrimitiveValue::TInteger);
    EXPECT_EQ(lref.getAsInt(), 3);

    // append
    ReferenceValue append = aval.getProperty("append", false);
    append.deref().value().getAsFunction()(table, {Value(4.f)}, ret);
    ASSERT_EQ(aval.value().getAsArray().size(), 4);

    // find
    ReferenceValue find = aval.getProperty("find", false);
    find.deref().value().getAsFunction()(table, {Value(4.f)}, ret);
    ASSERT_EQ(ret.value().getType(), PrimitiveValue::TInteger);
    EXPECT_EQ(ret.value().getAsInt(), 3);

    // insert
    ReferenceValue insert = aval.getProperty("insert", false);
    insert.deref().value().getAsFunction()(table, {Value(0), Value("str")}, ret);
    ASSERT_EQ(aval.value().getAsArray().size(), 5);
    ASSERT_EQ(aval.value().getAsArray()[0].value().getType(), PrimitiveValue::TString);
    ASSERT_EQ(aval.value().getAsArray()[0].value().getAsString(), "str");

    // erase
    ReferenceValue erase = aval.getProperty("erase", false);
    erase.deref().value().getAsFunction()(table, {Value(0)}, ret);
    ASSERT_EQ(aval.value().getAsArray().size(), 4);

    // resize
    ReferenceValue resize = aval.getProperty("resize", false);
    resize.deref().value().getAsFunction()(table, {Value(2)}, ret);
    ASSERT_EQ(aval.value().getAsArray().size(), 2);

    // clear
    ReferenceValue clear = aval.getProperty("clear", false);
    clear.deref().value().getAsFunction()(table, {}, ret);
    ASSERT_EQ(aval.value().getAsArray().size(), 0);

    // at and keys
    aval.setProperty("prop", Value("prop"));
    ReferenceValue keys = aval.getProperty("keys", false);
    keys.deref().value().getAsFunction()(table, {}, ret);
    ASSERT_EQ(ret.value().getType(), PrimitiveValue::TArray);
    ASSERT_EQ(ret.value().getAsArray().size(), 1);
    EXPECT_EQ(ret.value().getAsArray().front().value().getAsString(), "prop");

    ReferenceValue at = aval.getProperty("at", false);
    at.deref().value().getAsFunction()(table, {Value("prop")}, ret);
    ASSERT_EQ(ret.value().getType(), PrimitiveValue::TString);
    ASSERT_EQ(ret.value().getAsString(), "prop");
}

} // namespace unittest
} // namespace script
} // namespace bl
