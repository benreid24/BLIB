#include <BLIB/Util/EmplaceHelper.hpp>
#include <gtest/gtest.h>

#include <optional>

namespace bl
{
namespace util
{
namespace unittest
{
namespace
{
class PrivateConstructor {
public:
    static PrivateConstructor make(int code) { return PrivateConstructor(code); }

    int getCode() const { return code; }

private:
    int code;

    PrivateConstructor(int code)
    : code(code) {}
};
} // namespace

TEST(EmplaceHelper, Factory) {
    const int testCode = 5;
    std::optional<PrivateConstructor> test;
    test.emplace(EmplaceHelper([testCode]() { return PrivateConstructor::make(testCode); }));
    EXPECT_EQ(test.value().getCode(), testCode);
}

} // namespace unittest
} // namespace util
} // namespace bl
