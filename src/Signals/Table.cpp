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

struct TableData {
    std::mutex mutex;
    std::unordered_map<std::string, ChannelPtr> channelsByString;
    std::unordered_map<void*, ChannelPtr> channelsByPointer;
    Channel globalChannel;
} data;

void noopDelete(Channel*) {}
} // namespace

Channel& sig::Table::createChannel(const std::string& key) {
    std::unique_lock lock(data.mutex);

    auto it = data.channelsByString.find(key);
    if (it != data.channelsByString.end()) {
        BL_LOG_WARN << "Channel with key '" << key
                    << "' already exists, returning existing channel";
        return *it->second;
    }

    it = data.channelsByString.try_emplace(key, ChannelPtr(new Channel())).first;
    return *it->second;
}

Channel& Table::createChannel(void* key) {
    std::unique_lock lock(data.mutex);

    auto it = data.channelsByPointer.find(key);
    if (it != data.channelsByPointer.end()) {
        BL_LOG_WARN << "Channel with key '" << key
                    << "' already exists, returning existing channel";
        return *it->second;
    }

    it = data.channelsByPointer.try_emplace(key, ChannelPtr(new Channel())).first;
    return *it->second;
}

void Table::registerChannel(const std::string& key, Channel& channel) {
    std::unique_lock lock(data.mutex);

    const auto it = data.channelsByString.find(key);
    if (it != data.channelsByString.end()) {
        BL_LOG_WARN << "Channel with key '" << key
                    << "' already exists, replacing registered value";
        it->second = ChannelPtr(&channel, Deleter(false));
    }
    else { data.channelsByString.try_emplace(key, ChannelPtr(&channel, Deleter(false))); }
}

void Table::registerChannel(void* key, Channel& channel) {
    std::unique_lock lock(data.mutex);

    const auto it = data.channelsByPointer.find(key);
    if (it != data.channelsByPointer.end()) {
        BL_LOG_WARN << "Channel with key '" << key
                    << "' already exists, returning existing channel";
        it->second = ChannelPtr(&channel, Deleter(false));
    }
    else { data.channelsByPointer.try_emplace(key, ChannelPtr(&channel, Deleter(false))); }
}

Channel& Table::getChannel(const std::string& key) {
    std::unique_lock lock(data.mutex);

    const auto it = data.channelsByString.find(key);
    if (it != data.channelsByString.end()) { return *it->second; }

    BL_LOG_WARN << "Channel with key '" << key << "' does not exist, creating a new channel";
    lock.unlock();
    return createChannel(key);
}

Channel& Table::getChannel(void* key) {
    std::unique_lock lock(data.mutex);

    const auto it = data.channelsByPointer.find(key);
    if (it != data.channelsByPointer.end()) { return *it->second; }

    BL_LOG_WARN << "Channel with key '" << key << "' does not exist, creating a new channel";
    lock.unlock();
    return createChannel(key);
}

void Table::removeChannel(const std::string& key) {
    std::unique_lock lock(data.mutex);
    data.channelsByString.erase(key);
}

void Table::removeChannel(void* key) {
    std::unique_lock lock(data.mutex);
    data.channelsByPointer.erase(key);
}

Channel& Table::getGlobalChannel() { return data.globalChannel; }

} // namespace sig
} // namespace bl