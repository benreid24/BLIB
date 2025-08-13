#include <BLIB/Signals/Channel.hpp>

#include <BLIB/Signals/Emitter.hpp>

namespace bl
{
namespace sig
{
Channel::Channel()
: needDeferSync(false)
, streams()
, parent(nullptr)
, closed(false) {}

Channel::Channel(Channel&& channel) { *this = std::move(channel); }

Channel::~Channel() { shutdown(); }

Channel& Channel::operator=(Channel&& channel) {
    if (this == &channel) { return *this; }

    if (channel.closed) {
        BL_LOG_ERROR << "Cannot move from a shutdown channel";
        return *this;
    }

    std::unique_lock lock(mutex);
    streams        = std::move(channel.streams);
    parent         = channel.parent;
    children       = std::move(channel.children);
    channel.parent = nullptr;
    if (parent) { reparent(&channel); }
    return *this;
}

void Channel::syncDeferred() {
    if (needDeferSync) {
        needDeferSync = false;
        std::unique_lock lock(mutex);
        for (auto& stream : streams) { stream.second->syncDeferred(); }
    }
}

void Channel::setParent(Channel& p) {
    std::unique_lock lock(mutex);

    if (closed) {
        BL_LOG_ERROR << "Cannot set parent on a shutdown channel";
        return;
    }
    if (p.isShutdown()) {
        BL_LOG_ERROR << "Cannot become the child of a shutdown channel";
        return;
    }
    if (parent) {
        BL_LOG_ERROR << "Channel already has a parent";
        return;
    }

    parent = &p;
    std::unique_lock parentLock(p.mutex);
    p.children.push_back(this);
}

void Channel::addChild(Channel& child) { child.setParent(*this); }

void Channel::shutdown() {
    shutdownFromParent();

    if (parent) {
        std::unique_lock parentLock(parent->mutex);
        for (auto it = parent->children.begin(); it != parent->children.end(); ++it) {
            if (*it == this) {
                parent->children.erase(it);
                break;
            }
        }
        parent = nullptr;
    }
}

void Channel::shutdownFromParent() {
    std::unique_lock lock(mutex);

    closed = true;
    for (auto& stream : streams) { delete stream.second; }
    streams.clear();

    for (Channel* child : children) {
        child->shutdownFromParent();
        child->parent = nullptr;
    }
    children.clear();
}

void Channel::reparent(Channel* original) {
    if (parent) {
        for (Channel*& sibling : parent->children) {
            if (sibling == original) {
                sibling = this;
                break;
            }
        }

        for (Channel* child : children) { child->parent = this; }
    }
}

} // namespace sig
} // namespace bl
