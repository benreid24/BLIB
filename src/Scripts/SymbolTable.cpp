#include <BLIB/Scripts/SymbolTable.hpp>
#include <iostream>

namespace bl
{
namespace scripts
{
namespace
{
using TableKey =
    std::pair<unsigned int, std::unordered_map<std::string, Value::Ptr>::iterator>;

TableKey searchTable(const std::string& name,
                     std::vector<std::unordered_map<std::string, Value::Ptr>>& table) {
    for (int i = static_cast<int>(table.size() - 1); i >= 0; --i) {
        auto j = table[i].find(name);
        if (j != table[i].end()) return TableKey(i, j);
    }
    return TableKey(0, table[0].end());
}
} // namespace

SymbolTable::SymbolTable() { pushFrame(); }

void SymbolTable::pushFrame() { table.push_back({}); }

void SymbolTable::popFrame() {
    if (table.size() > 1)
        table.pop_back();
    else
        std::cerr << "Error: Attempted to pop global frame from SymbolTable" << std::endl;
}

bool SymbolTable::exists(const std::string& name) const {
    auto& tbl = const_cast<std::vector<std::unordered_map<std::string, Value::Ptr>>&>(table);
    TableKey key = searchTable(name, tbl);
    return table[key.first].end() != key.second;
}

Value::Ptr SymbolTable::get(const std::string& name) {
    TableKey key = searchTable(name, table);
    if (table[key.first].end() != key.second) return key.second->second;
    return nullptr;
}

void SymbolTable::set(const std::string& name, const Value& val) {
    TableKey key = searchTable(name, table);
    Value::Ptr value;
    if (table[key.first].end() != key.second)
        value = key.second->second;
    else {
        value.reset(new Value());
        table[table.size() - 1].insert(std::make_pair(name, value));
    }
    *value = val;
}

} // namespace scripts
} // namespace bl