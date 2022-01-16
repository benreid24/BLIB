#include <BLIB/Scripts/SymbolTable.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Scripts/Manager.hpp>

namespace bl
{
namespace script
{
SymbolTable::SymbolTable()
: mgr(nullptr)
, stop(false) {}

SymbolTable::SymbolTable(const SymbolTable& copy)
: global(copy.global)
, mgr(copy.mgr)
, stop(copy.stop.operator bool()) {}

void SymbolTable::copy(const SymbolTable& copy) {
    global = copy.global;
    if (copy.mgr) { mgr = copy.mgr; }
}

SymbolTable SymbolTable::base() const {
    SymbolTable t;
    t.global = global;
    t.mgr    = mgr;
    return t;
}

void SymbolTable::pushFrame() { stack.emplace(); }

void SymbolTable::popFrame() {
    if (!stack.empty()) { stack.pop(); }
    else {
        throw Error("Internal error: Tried to pop from empty stack");
    }
}

bool SymbolTable::exists(const std::string& name) const {
    const auto it = global.find(name);
    if (it != global.end()) return true;
    if (!stack.empty()) { return stack.top().find(name) != stack.top().end(); }
    return false;
}

ReferenceValue* SymbolTable::get(const std::string& name, bool create) {
    if (!stack.empty()) {
        auto it = stack.top().find(name);
        if (it == stack.top().end()) {
            if (create) {
                it = stack.top().emplace(name, Value()).first;
                return &it->second;
            }
        }
        else {
            return &it->second;
        }
    }

    auto it = global.find(name);
    if (it == global.end()) {
        if (create) {
            it = global.emplace(name, Value()).first;
            return &it->second;
        }
    }
    else {
        return &it->second;
    }
    return nullptr;
}

void SymbolTable::set(const std::string& name, const Value& val, bool top) {
    if (!stack.empty()) {
        auto it = stack.top().find(name);
        if (it != stack.top().end()) {
            it->second = val;
            return;
        }
    }

    auto it = global.find(name);
    if (it != global.end()) {
        if (top && !stack.empty()) { stack.top().emplace(name, val); }
        else {
            it->second = val;
        }
    }
    else if (!stack.empty()) {
        stack.top().emplace(name, val);
    }
    else {
        global.emplace(name, val);
    }
}

void SymbolTable::kill() {
    stop = true;
    waitVar.notify_all();
}

bool SymbolTable::killed() const { return stop; }

void SymbolTable::reset() {
    while (!stack.empty()) { stack.pop(); }
    global.clear();
}

void SymbolTable::registerManager(Manager* m) { mgr = m; }

Manager* SymbolTable::manager() { return mgr; }

void SymbolTable::waitFor(unsigned long int ms) {
    std::unique_lock lock(waitMutex);
    waitVar.wait_for(lock, std::chrono::milliseconds(ms));
}

void SymbolTable::waitOn(util::Waiter& waiter) {
    while (!waiter.wasUnblocked()) {
        waiter.waitFor(std::chrono::milliseconds(100));
        if (killed()) throw Exit();
    }
}

} // namespace script
} // namespace bl
