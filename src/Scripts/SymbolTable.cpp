#include <BLIB/Scripts/SymbolTable.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Scripts/Manager.hpp>

namespace bl
{
namespace script
{
namespace
{
using TableKey = std::pair<unsigned int, std::unordered_map<std::string, Value::Ptr>::iterator>;

TableKey searchTable(const std::string& name,
                     std::vector<std::unordered_map<std::string, Value::Ptr>>& table,
                     bool topOnly = false) {
    for (int i = static_cast<int>(table.size() - 1); i >= 0; --i) {
        auto j = table[i].find(name);
        if (j != table[i].end()) return TableKey(i, j);
        if (topOnly) break;
    }
    return TableKey(0, table[0].end());
}
} // namespace

SymbolTable::SymbolTable()
: mgr(nullptr)
, stop(false) {
    pushFrame();
}

SymbolTable::SymbolTable(const SymbolTable& copy)
: table(copy.table)
, mgr(copy.mgr)
, stop(copy.stop.operator bool()) {}

void SymbolTable::copy(const SymbolTable& copy) { table = copy.table; }

SymbolTable SymbolTable::base() const {
    SymbolTable derived;
    derived.table.emplace_back(table.front());
    return derived;
}

void SymbolTable::pushFrame() { table.emplace_back(); }

void SymbolTable::popFrame() {
    if (table.size() > 1)
        table.pop_back();
    else
        BL_LOG_WARN << "Attempted to pop global frame from SymbolTable";
}

bool SymbolTable::exists(const std::string& name) const {
    auto& tbl    = const_cast<std::vector<std::unordered_map<std::string, Value::Ptr>>&>(table);
    TableKey key = searchTable(name, tbl);
    return table[key.first].end() != key.second;
}

Value::Ptr SymbolTable::get(const std::string& name, bool create) {
    TableKey key = searchTable(name, table);
    if (table[key.first].end() != key.second) return key.second->second;
    if (create) {
        set(name, Value(), true);
        return get(name, false);
    }
    return nullptr;
}

void SymbolTable::set(const std::string& name, const Value& val, bool top) {
    TableKey key = searchTable(name, table, top);
    Value::Ptr value;
    if (table[key.first].end() != key.second)
        value = key.second->second;
    else {
        value.reset(new Value());
        table[table.size() - 1].insert(std::make_pair(name, value));
    }
    *value = val;
}

void SymbolTable::kill() {
    stop = true;
    waitVar.notify_all();
}

bool SymbolTable::killed() const { return stop; }

void SymbolTable::reset() {
    table.clear();
    pushFrame();
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
