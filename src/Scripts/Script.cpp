#include <BLIB/Scripts.hpp>

#include <BLIB/Files/FileUtil.hpp>
#include <Scripts/Parser.hpp>
#include <Scripts/ScriptImpl.hpp>
#include <Scripts/ScriptLibrary.hpp>
#include <fstream>
#include <iostream>
#include <streambuf>

namespace bl
{
using namespace scripts;

Script::Script(const std::string& data)
: source(data) {
    std::string input = data;
    if (FileUtil::exists(data)) {
        std::ifstream file(data.c_str());
        file.seekg(0, std::ios::end);
        input.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        input.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    root = scripts::Parser::parse(input);
}

bool Script::valid() const { return root.get() != nullptr; }

std::optional<scripts::Value> Script::run(ScriptManager* manager) const {
    if (!valid()) return {};
    ExecutionContext::Ptr ctx(new ExecutionContext(root));
    ctx->table = generateBaseTable();
    if (manager) ctx->table.registerManager(manager);
    addCustomSymbols(ctx->table);
    onRun();
    return execute(ctx);
}

void Script::runBackground(ScriptManager* manager) const {
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

std::optional<scripts::Value> Script::execute(ExecutionContext::Ptr context) const {
    try {
        std::optional<scripts::Value> r =
            ScriptImpl::runStatementList(context->root->children[0], context->table);
        context->running = false;
        context.reset();
        return r.value_or(Value());
    } catch (const Error& err) {
        context->running = false;
        context.reset();
        std::cerr << err.stacktrace() << std::endl;
        return {};
    } catch (const Exit&) { return {}; }
}

SymbolTable Script::generateBaseTable() const {
    SymbolTable table;
    Library::addBuiltIns(table);
    return table;
}

} // namespace bl