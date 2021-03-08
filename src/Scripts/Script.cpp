#include <BLIB/Scripts.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Files/Util.hpp>
#include <BLIB/Logging.hpp>
#include <Scripts/Parser.hpp>
#include <Scripts/ScriptImpl.hpp>
#include <Scripts/ScriptLibrary.hpp>
#include <fstream>
#include <streambuf>

namespace bl
{
namespace script
{
Script::Script(const std::string& data)
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
}

bool Script::valid() const { return root.get() != nullptr; }

std::optional<script::Value> Script::run(engine::Engine& engine) const {
    return run(&engine.scriptManager());
}

std::optional<script::Value> Script::run(Manager* manager) const {
    if (!valid()) return {};
    ExecutionContext::Ptr ctx(new ExecutionContext(root));
    ctx->table = generateBaseTable();
    if (manager) ctx->table.registerManager(manager);
    addCustomSymbols(ctx->table);
    onRun();
    return execute(ctx);
}

void Script::runBackground(engine::Engine& engine) const { runBackground(&engine.scriptManager()); }

void Script::runBackground(Manager* manager) const {
    if (!valid()) return;
    ExecutionContext::Ptr ctx(new ExecutionContext(root));
    ctx->table = generateBaseTable();
    if (manager) ctx->table.registerManager(manager);
    addCustomSymbols(ctx->table);
    onRun();
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

SymbolTable Script::generateBaseTable() const {
    SymbolTable table;
    Library::addBuiltIns(table);
    return table;
}

} // namespace script
} // namespace bl
