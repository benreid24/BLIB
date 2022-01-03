#include <BLIB/Scripts/SymbolTable.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Scripts/Manager.hpp>

namespace bl
{
namespace script
{
namespace
{
Value::Ref searchTable(const std::string& name,
                       std::vector<std::unordered_map<std::string, Value>>& table,
                       bool topOnly = false) {
    for (int i = static_cast<int>(table.size() - 1); i >= 0; --i) {
        auto j = table[i].find(name);
        if (j != table[i].end()) return {&j->second, i};
        if (topOnly) break;
    }
    return {};
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

void SymbolTable::pushFrame() {
    table.emplace_back();
    props.emplace_back();
}

void SymbolTable::popFrame() {
    if (table.size() > 1) {
        table.pop_back();
        props.pop_back();
    }
    else
        BL_LOG_WARN << "Attempted to pop global frame from SymbolTable";
}

bool SymbolTable::exists(const std::string& name) const {
    auto& tbl      = const_cast<std::vector<std::unordered_map<std::string, Value>>&>(table);
    Value::Ref ref = searchTable(name, tbl);
    return ref.value != nullptr;
}

Value::Ref SymbolTable::get(const std::string& name, bool create) {
    Value::Ref ref = searchTable(name, table);
    if (ref.value) return ref;
    if (create) {
        auto it          = table.back().try_emplace(name).first;
        it->second.depth = table.size() - 1;
        return {&it->second, currentDepth()};
    }
    return {};
}

void SymbolTable::set(const std::string& name, const Value& val, bool top) {
    Value::Ref ref = searchTable(name, table, top);
    if (ref.value) { *ref.value = val; }
    else {
        auto it          = table.back().emplace(name, val).first;
        it->second.depth = currentDepth();
    }
}

Value* SymbolTable::getProp(const Value::Ref& ref, const std::string& name, bool create) {
    if (ref.depth < 0 || ref.depth >= static_cast<signed>(props.size())) return nullptr;

    auto it = props[ref.depth].find(ref.value);
    if (it == props[ref.depth].end()) { it = props[ref.depth].try_emplace(ref.value).first; }

    const auto bit = Value::Builtins.find(name);
    if (bit != Value::Builtins.end()) {
        auto pit = it->second.find(name);
        if (pit == it->second.end()) {
            pit =
                it->second
                    .emplace(
                        name,
                        Function(std::bind(
                            bit->second, ref.value, std::placeholders::_1, std::placeholders::_2)))
                    .first;
        }
        return &pit->second;
    }

    if (name == "length") {
        auto pit = it->second.find(name);
        if (pit == it->second.end()) { pit = it->second.try_emplace(name).first; }
        if (ref.value->getType() == Value::TArray) {
            pit->second = static_cast<float>(ref.value->getAsArray().size());
        }
        else {
            pit->second = 0.f;
        }
        return &pit->second;
    }

    auto pit = it->second.find(name);
    if (pit != it->second.end()) return &pit->second;
    if (create) {
        pit = it->second.try_emplace(name).first;
        return &pit->second;
    }
    return nullptr;
}

Value* SymbolTable::setProp(const Value::Ref& ref, const std::string& name, const Value& val) {
    if (ref.depth < 0 || ref.depth >= static_cast<signed>(props.size())) return nullptr;

    if (name == "length" || Value::Builtins.find(name) != Value::Builtins.end()) {
        throw Error("Writing to read-only property: '" + name + "'");
    }

    auto it = props[ref.depth].find(ref.value);
    if (it == props[ref.depth].end()) { it = props[ref.depth].try_emplace(ref.value).first; }
    auto pit = it->second.find(name);
    if (pit == it->second.end()) { pit = it->second.try_emplace(name).first; }
    pit->second = val;
    return &pit->second;
}

const std::unordered_map<std::string, Value>& SymbolTable::getAllProps(const Value& val) const {
    static const std::unordered_map<std::string, Value> empty;

    if (val.depth < 0 || val.depth >= static_cast<signed>(props.size())) return empty;
    auto it = props[val.depth].find(&val);
    if (it == props[val.depth].end()) return empty;
    return it->second;
}

int SymbolTable::currentDepth() const { return static_cast<int>(table.size() - 1); }

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
