#include <BLIB/Signals/Table.hpp>

#include <BLIB/Logging.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace bl
{
namespace sig
{
namespace
{
struct Deleter {
    Deleter(bool release = true)
    : release(release) {}

    void operator()(Channel* channel) {
        if (release) { delete channel; }
    }

private:
    bool release;
};

using ChannelPtr = std::unique_ptr<Channel, Deleter>;

std::mutex mutex;
std::unordered_map<std::string, ChannelPtr> channelsByString;
std::unordered_map<void*, ChannelPtr> channelsByPointer;

void noopDelete(Channel*) {}
} // namespace

Channel& sig::Table::createChannel(const std::string& key) {
    std::unique_lock lock(mutex);

    auto it = channelsByString.find(key);
    if (it != channelsByString.end()) {
        BL_LOG_WARN << "Channel with key '" << key
                    << "' already exists, returning existing channel";
        return *it->second;
    }

    it = channelsByString.try_emplace(key, ChannelPtr(new Channel())).first;
    return *it->second;
}

Channel& Table::createChannel(void* key) {
    std::unique_lock lock(mutex);

    auto it = channelsByPointer.find(key);
    if (it != channelsByPointer.end()) {
        BL_LOG_WARN << "Channel with key '" << key
                    << "' already exists, returning existing channel";
        return *it->second;
    }

    it = channelsByPointer.try_emplace(key, ChannelPtr(new Channel())).first;
    return *it->second;
}

void Table::registerChannel(const std::string& key, Channel& channel) {
    std::unique_lock lock(mutex);

    const auto it = channelsByString.find(key);
    if (it != channelsByString.end()) {
        BL_LOG_WARN << "Channel with key '" << key
                    << "' already exists, replacing registered value";
        it->second = ChannelPtr(&channel, Deleter(false));
    }
    else { channelsByString.try_emplace(key, ChannelPtr(&channel, Deleter(false))); }
}

void Table::registerChannel(void* key, Channel& channel) {
    std::unique_lock lock(mutex);

    const auto it = channelsByPointer.find(key);
    if (it != channelsByPointer.end()) {
        BL_LOG_WARN << "Channel with key '" << key
                    << "' already exists, returning existing channel";
        it->second = ChannelPtr(&channel, Deleter(false));
    }
    else { channelsByPointer.try_emplace(key, ChannelPtr(&channel, Deleter(false))); }
}

Channel& Table::getChannel(const std::string& key) {
    std::unique_lock lock(mutex);

    const auto it = channelsByString.find(key);
    if (it != channelsByString.end()) { return *it->second; }

    BL_LOG_WARN << "Channel with key '" << key << "' does not exist, creating a new channel";
    lock.unlock();
    return createChannel(key);
}

Channel& Table::getChannel(void* key) {
    std::unique_lock lock(mutex);

    const auto it = channelsByPointer.find(key);
    if (it != channelsByPointer.end()) { return *it->second; }

    BL_LOG_WARN << "Channel with key '" << key << "' does not exist, creating a new channel";
    lock.unlock();
    return createChannel(key);
}

void Table::removeChannel(const std::string& key) {
    std::unique_lock lock(mutex);
    channelsByString.erase(key);
}

void Table::removeChannel(void* key) {
    std::unique_lock lock(mutex);
    channelsByPointer.erase(key);
}

} // namespace sig
} // namespace bl