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
    EXPECT_EQ(&bval.deref(), &bval);

    const Value nval(56.f);
    ASSERT_EQ(nval.value().getType(), PrimitiveValue::TNumeric);
    EXPECT_EQ(nval.value().getAsNum(), 56.f);

    Value sval("str");
    ASSERT_EQ(sval.value().getType(), PrimitiveValue::TString);
    EXPECT_EQ(sval.value().getAsString(), "str");

    const Value aval(ArrayValue{bval, nval, sval});
    ASSERT_EQ(aval.value().getType(), PrimitiveValue::TArray);
    const auto& arr = aval.value().getAsArray();
    ASSERT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0].value().getType(), PrimitiveValue::TBool);
    EXPECT_EQ(arr[1].value().getType(), PrimitiveValue::TNumeric);
    EXPECT_EQ(arr[2].value().getType(), PrimitiveValue::TString);

    const Value rval(ReferenceValue{&sval});
    ASSERT_EQ(rval.value().getType(), PrimitiveValue::TRef);
    ASSERT_EQ(&rval.deref(), &sval);
}

TEST(ScriptValue, GeneralProps) {
    const Value bval(true);
    const Value nval(56.f);
    const Value sval("str");

    Value value;
    value.setProperty("bval", bval);
    value.setProperty("nval", nval);
    value.setProperty("sval", sval);

    const auto& bref = value.getProperty("bval").deref().value();
    ASSERT_EQ(bref.getType(), PrimitiveValue::TBool);
    EXPECT_EQ(bref.getAsBool(), true);

    const auto& nref = value.getProperty("nval").deref().value();
    ASSERT_EQ(nref.getType(), PrimitiveValue::TNumeric);
    EXPECT_EQ(nref.getAsNum(), 56.f);

    const auto& sref = value.getProperty("sval").deref().value();
    ASSERT_EQ(sref.getType(), PrimitiveValue::TString);
    EXPECT_EQ(sref.getAsString(), "str");

    EXPECT_THROW(value.getProperty("dne"), Error);
}

TEST(ScriptValue, Builtins) {
    SymbolTable table;
    Value aval(ArrayValue{Value(1.f), Value(2.f), Value(3.f)});

    // length
    ReferenceValue len = aval.getProperty("length");
    const auto& lref   = len.deref().value();
    ASSERT_EQ(lref.getType(), PrimitiveValue::TNumeric);
    EXPECT_EQ(lref.getAsNum(), 3.f);

    // append
    ReferenceValue append = aval.getProperty("append");
    append.deref().value().getAsFunction()(table, {Value(4.f)});
    ASSERT_EQ(aval.value().getAsArray().size(), 4);

    // find
    ReferenceValue find = aval.getProperty("find");
    Value i             = find.deref().value().getAsFunction()(table, {Value(4.f)});
    ASSERT_EQ(i.value().getType(), PrimitiveValue::TNumeric);
    EXPECT_EQ(i.value().getAsNum(), 3.f);

    // insert
    ReferenceValue insert = aval.getProperty("insert");
    insert.deref().value().getAsFunction()(table, {Value(0.f), Value("str")});
    ASSERT_EQ(aval.value().getAsArray().size(), 5);
    ASSERT_EQ(aval.value().getAsArray()[0].value().getType(), PrimitiveValue::TString);
    ASSERT_EQ(aval.value().getAsArray()[0].value().getAsString(), "str");

    // erase
    ReferenceValue erase = aval.getProperty("erase");
    erase.deref().value().getAsFunction()(table, {Value(0.f)});
    ASSERT_EQ(aval.value().getAsArray().size(), 4);

    // resize
    ReferenceValue resize = aval.getProperty("resize");
    resize.deref().value().getAsFunction()(table, {Value(2.f)});
    ASSERT_EQ(aval.value().getAsArray().size(), 2);

    // clear
    ReferenceValue clear = aval.getProperty("clear");
    clear.deref().value().getAsFunction()(table, {});
    ASSERT_EQ(aval.value().getAsArray().size(), 0);

    // at and keys
    aval.setProperty("prop", Value("prop"));
    ReferenceValue keys = aval.getProperty("keys");
    Value keysResult    = keys.deref().deref().value().getAsFunction()(table, {});
    ASSERT_EQ(keysResult.value().getType(), PrimitiveValue::TArray);
    ASSERT_EQ(keysResult.value().getAsArray().size(), 1);
    EXPECT_EQ(keysResult.value().getAsArray().front().value().getAsString(), "prop");

    ReferenceValue at = aval.getProperty("at");
    Value atResult    = at.deref().value().getAsFunction()(table, {Value("prop")});
    ASSERT_EQ(atResult.value().getType(), PrimitiveValue::TRef);
    ASSERT_EQ(atResult.deref().value().getType(), PrimitiveValue::TString);
    ASSERT_EQ(atResult.deref().value().getAsString(), "prop");
}

} // namespace unittest
} // namespace script
} // namespace bl
