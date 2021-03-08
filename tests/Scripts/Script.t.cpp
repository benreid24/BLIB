#include <BLIB/Scripts.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
namespace unittest
{
class TestScript : public Script {
public:
    TestScript(const std::string& input)
    : Script(input)
    , ran(false)
    , call(false) {}

    bool called() const { return call; }
    bool executed() const { return ran; }

private:
    virtual void addCustomSymbols(script::SymbolTable& table) const override {
        TestScript* me        = const_cast<TestScript*>(this);
        Function::CustomCB cb = [me](SymbolTable&, const std::vector<Value>&) -> Value {
            me->call = true;
            return Value();
        };
        table.set("call", Value(cb));
    }

    virtual void onRun() const override { ran = true; }

    mutable bool ran;
    bool call;
};

TEST(Script, Run) {
    const std::string input = "def square(x) { return x*x; } y = 4; return square(y);";
    const Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.getType(), Value::TNumeric);
    ASSERT_EQ(v.getAsNum(), 16);
}

TEST(Script, Background) {
    const std::string input = "call();";
    const TestScript script(input);
    ASSERT_TRUE(script.valid());

    script.runBackground();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_TRUE(script.executed());
    ASSERT_TRUE(script.called());
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
    ASSERT_EQ(v.getType(), Value::TNumeric);
    ASSERT_EQ(v.getAsNum(), 10);
}

} // namespace unittest
} // namespace script
} // namespace bl
