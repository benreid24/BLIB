#ifndef BLIB_SIGNALS_TABLE_HPP
#define BLIB_SIGNALS_TABLE_HPP

#include <BLIB/Signals/Channel.hpp>
#include <string>

namespace bl
{
namespace sig
{
class Table {
public:
    static Channel& createChannel(const std::string& key);

    static Channel& createChannel(void* key);

    static void registerChannel(const std::string& key, Channel& channel);

    static void registerChannel(void* key, Channel& channel);

    static Channel& getChannel(const std::string& key);

    static Channel& getChannel(void* key);

    static void removeChannel(const std::string& key);

    static void removeChannel(void* key);
};

} // namespace sig
} // namespace bl

#endif
