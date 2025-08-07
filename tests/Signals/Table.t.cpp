#include <BLIB/Signals.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace sig
{
namespace unittest
{
namespace
{
class IntListener : public Listener<int> {
public:
    virtual ~IntListener() = default;

    MOCK_METHOD(void, process, (const int&), (override));
};
} // namespace
TEST(Table, CreateChannel) {
    Channel& channel  = Table::createChannel("test_channel");
    Channel& channel2 = Table::createChannel("test_channel2");

    EXPECT_NE(&channel, &channel2);
    EXPECT_EQ(&Table::getChannel("test_channel"), &channel);
    EXPECT_EQ(&Table::getChannel("test_channel2"), &channel2);
}

TEST(Table, RegisterChannel) {
    Channel channel;
    Table::registerChannel("registered_channel", channel);
    EXPECT_EQ(&Table::getChannel("registered_channel"), &channel);
    Channel channel2;
    Table::registerChannel("registered_channel", channel2);
    EXPECT_EQ(&Table::getChannel("registered_channel"), &channel2);
}

TEST(Table, GetChannel) {
    Channel& channel = Table::getChannel("get_channel");
    EXPECT_EQ(&channel, &Table::getChannel("get_channel"));
    EXPECT_EQ(&Table::getChannel("get_channel2"), &Table::createChannel("get_channel2"));
}

TEST(Table, RemoveChannel) {
    Channel& channel = Table::createChannel("remove_channel");
    IntListener listener;
    listener.subscribe(channel);

    EXPECT_EQ(&Table::getChannel("remove_channel"), &channel);
    EXPECT_TRUE(listener.isSubscribed());

    Table::removeChannel("remove_channel");
    EXPECT_FALSE(listener.isSubscribed());
}

} // namespace unittest
} // namespace sig
} // namespace bl
