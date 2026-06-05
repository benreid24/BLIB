#include <BLIB/Scripts.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
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
    root = script::Parser::parse(data, &error);

    if (addDefaults) { Context().initializeTable(defaultTable); }
}

Script::Script(as::TypedRef<asi::FilePayload> file)
: Script(std::string(file->getData().data(), file->getData().size()), true) {}

Script::Script(as::TypedRef<asi::FilePayload> file, const Context& ctx)
: Script(std::string(file->getData().data(), file->getData().size()), false) {
    ctx.initializeTable(defaultTable);
}

Script::Script(as::TypedRef<asi::FilePayload> file, const SymbolTable& t)
: Script(std::string(file->getData().data(), file->getData().size()), false) {
    defaultTable.copy(t);
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

const std::string& Script::errorMessage() const { return error; }

void Script::resetContext(const Context& ctx, bool clear) {
    if (clear) { defaultTable.reset(); }
    ctx.initializeTable(defaultTable);
}

std::optional<script::Value> Script::run(Manager* manager) const {
    if (!valid()) return {};
    ExecutionContext::Ptr ctx(new ExecutionContext(root, defaultTable, source));
    if (manager) ctx->table.registerManager(manager);
    return execute(ctx);
}

void Script::runBackground(Manager* manager) const {
    if (!valid()) return;
    ExecutionContext::Ptr ctx(new ExecutionContext(root, defaultTable, source));
    if (manager) ctx->table.registerManager(manager);
    ctx->thread.reset(new std::thread(&Script::execute, ctx));
    ctx->thread->detach();
    if (manager) manager->watch(ctx);
}

std::optional<script::Value> Script::execute(ExecutionContext::Ptr context) {
    try {
        std::optional<script::Value> r =
            ScriptImpl::runStatementList(context->root->children[0], context->table);
        context->running = false;
        context.reset();
        return r.value_or(Value());
    } catch (const Error& err) {
        context->running = false;
        BL_LOG_ERROR << "Error in script '" << context->source << "':\n" << err.stacktrace();
        context.reset();
        return {};
    } catch (const Exit&) { return {}; }
}

} // namespace script
} // namespace bl
