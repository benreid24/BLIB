#ifndef BLIB_SCRIPTS_SCRIPT_HPP
#define BLIB_SCRIPTS_SCRIPT_HPP

#include <BLIB/Parser/Node.hpp>
#include <BLIB/Scripts/Context.hpp>
#include <BLIB/Scripts/SymbolTable.hpp>

#include <atomic>
#include <mutex>
#include <optional>
#include <thread>

namespace bl
{
namespace engine
{
class Engine;
}

/// Script interpreter for bScript
namespace script
{
class Manager;

/**
 * @brief Loads scripts. Can run scripts directly or in the background. Is threadsafe
 * @ingroup Scripts
 *
 */
class Script {
public:
    /**
     * @brief Determines if the input is a file, then loads the script from the file or from
     *        the input directly
     *
     * @param data The script content to run, or a filename containing a script
     * @param ctx Context to initialize the default symbol table with
     */
    Script(const std::string& data);

    /**
     * @brief Determines if the input is a file, then loads the script from the file or from
     *        the input directly. Uses a custom context to initialize built-in functions
     *
     * @param data The script content to run, or a filename containing a script
     * @param ctx Context to initialize the default symbol table with
     */
    Script(const std::string& data, const Context& ctx);

    /**
     * @brief Special constructor that takes an assembled table and uses that instead of using a
     *        context to add built-in functions
     *
     * @param data The script content to run, or a filename containing a script
     * @param startingTable The symbol table to begin with
     */
    Script(const std::string& script, const SymbolTable& startingTable);

    /**
     * @brief Returns if the Script is valid or not
     *
     */
    bool valid() const;

    /**
     * @brief Runs the script in the current thread
     *
     * @param manager The Manager to use to manage spawned Scripts
     * @return Return value if completed successfully (void if no return) or null on error
     *
     */
    std::optional<script::Value> run(Manager* manager = nullptr) const;

    /**
     * @brief Runs the script in the background
     *
     * @param manager Manager to register scripts with. Optional
     *
     */
    void runBackground(Manager* manager = nullptr) const;

private:
    parser::Node::Ptr root;
    const std::string source;
    SymbolTable defaultTable;

    struct ExecutionContext : public std::enable_shared_from_this<ExecutionContext> {
        typedef std::shared_ptr<ExecutionContext> Ptr;
        typedef std::weak_ptr<ExecutionContext> WPtr;

        parser::Node::Ptr root;
        std::shared_ptr<std::thread> thread;
        script::SymbolTable table;
        std::atomic_bool running;

        ExecutionContext(parser::Node::Ptr root, const SymbolTable& table)
        : root(root)
        , table(table)
        , running(true) {}
    };

    Script(const std::string& data, bool addDefaults);
    std::optional<script::Value> execute(ExecutionContext::Ptr context) const;

    friend class Manager;
};

} // namespace script
} // namespace bl

#endif
