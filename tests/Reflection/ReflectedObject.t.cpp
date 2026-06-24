#include <BLIB/Reflection.hpp>
#include <gtest/gtest.h>
#include <string>

namespace bl
{
namespace refl
{
namespace unittest
{
namespace
{
struct BasicObject {
    int a;
    float b;
    std::string c;
};
} // namespace
} // namespace unittest

template<>
struct ReflectedObject<unittest::BasicObject> {
    inline static const auto spec = makeSpec<unittest::BasicObject>(
        memberList(defineMember(1, "a", &unittest::BasicObject::a, attr::withDefaultValue(0)),
                   defineMember(2, "b", &unittest::BasicObject::b),
                   defineMember(3, "c", &unittest::BasicObject::c)),
        attr::withDefaultValue(unittest::BasicObject{0, 0.0f, ""}));
};

namespace unittest
{

TEST(ReflectedObject, BasicFields) {
    BasicObject obj{42, 3.14f, "hello"};

    BasicObject visited;
    visit(obj, [&](const auto& member, auto& value) { member.getMember(visited) = value; });
    EXPECT_EQ(visited.a, 42);
    EXPECT_FLOAT_EQ(visited.b, 3.14f);
    EXPECT_EQ(visited.c, "hello");
}

namespace
{
struct TemplateVisitor {
    BasicObject result;

    template<typename ObjectType, typename MemberType, typename... Attributes>
    void operator()(const ReflectedMember<ObjectType, MemberType, Attributes...>& member,
                    MemberType& value) {
        member.getMember(result) = value;
        if constexpr (member.hasAttribute<attr::DefaultValue<MemberType>>()) {
            const auto* defaultAttr = member.getAttribute<attr::DefaultValue<MemberType>>();
            if (defaultAttr) { member.getMember(result) = defaultAttr->value; }
        }
    }
};

} // namespace

TEST(ReflectedObject, TemplateVisitor) {
    BasicObject expected{16, 3.14f, "hello"};

    TemplateVisitor visitor;
    visit(expected, visitor);
    EXPECT_EQ(visitor.result.a, 0); // defaulted
    EXPECT_FLOAT_EQ(visitor.result.b, 3.14f);
    EXPECT_EQ(visitor.result.c, "hello");
}

TEST(ReflectedObject, DefaultValueAttribute) {
    constexpr bool testWorks =
        Attributes::hasAttribute<BasicObject, attr::DefaultValue<BasicObject>>();
    ASSERT_TRUE(testWorks);

    const attr::DefaultValue<BasicObject>* defaultAttr =
        Attributes::getAttribute<BasicObject, attr::DefaultValue<BasicObject>>();
    EXPECT_EQ(defaultAttr->value.a, 0);
    EXPECT_FLOAT_EQ(defaultAttr->value.b, 0.0f);
    EXPECT_EQ(defaultAttr->value.c, "");
}

} // namespace unittest
} // namespace refl
} // namespace bl
