#include <BLIB/Scripts.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
namespace unittest
{
namespace
{
class TestContext : public Context {
public:
    TestContext()
    : call(false) {}

    virtual ~TestContext() = default;

    bool called() const { return call; }

private:
    bool call;

    virtual void addCustomSymbols(SymbolTable& table) const override {
        TestContext* me       = const_cast<TestContext*>(this);
        Function::CustomCB cb = [me](SymbolTable&, const std::vector<Value>&, Value&) {
            me->call = true;
        };
        table.set("call", Value(Function(cb)));
    }
};
} // namespace

TEST(Script, Run) {
    const std::string input = "def square(x) { return x*x; } y = 4; return square(y);";
    const Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(v.value().getAsInt(), 16);
}

TEST(Script, Background) {
    const std::string input = "call();";
    TestContext context;
    const Script script(input, context);
    ASSERT_TRUE(script.valid());

    script.runBackground();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_TRUE(context.called());
}

TEST(Script, Manager) {
    const std::string input = "def square(x) { return x*x; } y = 4; return square(y);";
    const Script script(input);
    ASSERT_TRUE(script.valid());

    Manager manager;
    for (int i = 0; i < 5; ++i) script.runBackground(&manager);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_TRUE(manager.terminateAll());
}

TEST(Script, Shadow) {
    const std::string input = "x = 5; def f(x) { return x;} return f(10);";
    const Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(v.value().getAsInt(), 10);
}

} // namespace unittest
} // namespace script
} // namespace bl
