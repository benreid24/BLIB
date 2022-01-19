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
namespace
{
bool prepScript(std::string& script) {
    static const auto exists = [](const std::string& v) {
        if (util::FileUtil::exists(v)) {
            if (util::FileUtil::getExtension(v) != "bs") {
                BL_LOG_WARN << "bScript files should have '.bs' extension: " << v;
            }
            return true;
        }
        return false;
    };
    static const std::string path =
        engine::Configuration::getOrDefault<std::string>("blib.script.path", "");

    if (exists(script)) return true;
    script = util::FileUtil::joinPath(path, script);
    if (exists(script)) return true;
    return false;
}
} // namespace

Script::Script(const std::string& data, bool addDefaults)
: source(data) {
    std::string input = data;
    if (prepScript(input)) {
        BL_LOG_DEBUG << "Loading bScript: " << input;
        std::string content;
        util::FileUtil::readFile(input, content);
        root = script::Parser::parse(content, &error);
    }
    else {
        BL_LOG_DEBUG << "Loading bScript: " << data;
        root = script::Parser::parse(data, &error);
    }

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
