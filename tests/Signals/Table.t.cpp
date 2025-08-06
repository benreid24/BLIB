#include <BLIB/Signals/Table.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace sig
{
namespace unittest
{
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
    EXPECT_EQ(&Table::getChannel("remove_channel"), &channel);
    Table::removeChannel("remove_channel");
    EXPECT_NE(&Table::getChannel("remove_channel"), &channel);
}

} // namespace unittest
} // namespace sig
} // namespace bl
