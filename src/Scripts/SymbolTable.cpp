#include <BLIB/Scripts/SymbolTable.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Scripts/ScriptManager.hpp>

namespace bl
{
namespace scripts
{
namespace
{
using TableKey =
    std::pair<unsigned int, std::unordered_map<std::string, Value::Ptr>::iterator>;

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

void SymbolTable::pushFrame() { table.push_back({}); }

void SymbolTable::popFrame() {
    if (table.size() > 1)
        table.pop_back();
    else
        BL_LOG_WARN << "Attempted to pop global frame from SymbolTable";
}

bool SymbolTable::exists(const std::string& name) const {
    auto& tbl = const_cast<std::vector<std::unordered_map<std::string, Value::Ptr>>&>(table);
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

void SymbolTable::kill() { stop = true; }

bool SymbolTable::killed() const { return stop; }

void SymbolTable::registerManager(ScriptManager* m) { mgr = m; }

ScriptManager* SymbolTable::manager() { return mgr; }

} // namespace scripts
} // namespace bl