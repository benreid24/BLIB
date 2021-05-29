#include <BLIB/Scripts.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Files/Util.hpp>
#include <BLIB/Logging.hpp>
#include <Scripts/Parser.hpp>
#include <Scripts/ScriptImpl.hpp>
#include <fstream>
#include <streambuf>

namespace bl
{
namespace script
{
Script::Script(const std::string& data, bool addDefaults)
: source(data) {
    std::string input = data;
    if (file::Util::exists(data)) {
        std::ifstream file(data.c_str());
        file.seekg(0, std::ios::end);
        input.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        input.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    root = script::Parser::parse(input);
    if (addDefaults) Context().initializeTable(defaultTable);
}

Script::Script(const std::string& data)
: Script(data, true) {}

Script::Script(const std::string& data, const Context& ctx)
: Script(data, false) {
    ctx.initializeTable(defaultTable);
}

Script::Script(const std::string& s, const SymbolTable& t)
: Script(s, false) {
    defaultTable.copy(t);
}

bool Script::valid() const { return root.get() != nullptr; }

std::optional<script::Value> Script::run(Manager* manager) const {
    if (!valid()) return {};
    ExecutionContext::Ptr ctx(new ExecutionContext(root, defaultTable));
    if (manager) ctx->table.registerManager(manager);
    return execute(ctx);
}

void Script::runBackground(Manager* manager) const {
    if (!valid()) return;
    ExecutionContext::Ptr ctx(new ExecutionContext(root, defaultTable));
    if (manager) ctx->table.registerManager(manager);
    ctx->thread.reset(new std::thread(&Script::execute, this, ctx));
    ctx->thread->detach();
    if (manager) manager->watch(ctx);
}

std::optional<script::Value> Script::execute(ExecutionContext::Ptr context) const {
    try {
        std::optional<script::Value> r =
            ScriptImpl::runStatementList(context->root->children[0], context->table);
        context->running = false;
        context.reset();
        return r.value_or(Value());
    } catch (const Error& err) {
        context->running = false;
        context.reset();
        BL_LOG_ERROR << err.stacktrace();
        return {};
    } catch (const Exit&) { return {}; }
}

} // namespace script
} // namespace bl
