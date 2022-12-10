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
     * @brief Modifies the given script to its full file path and returns true if the script points
     *        to a file. If the file is not found then the input is not modified and false is
     *        returned
     *
     * @param path The script to try and find the full path for
     * @return True if the script points to a file, false otherwise
     */
    static bool getFullScriptPath(std::string& path);

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
     * @brief Returns an error message for the last parsing error that occured
     *
     */
    const std::string& errorMessage() const;

    /**
     * @brief Recreates the script's starting symbol table from a new context. Optionally clears the
     *        existing symbol table
     *
     * @param context Context to add built-in symbols from
     * @param clear True to clear the symbol table first, false to add/overrwrite into the existing
     */
    void resetContext(const Context& context, bool clear = false);

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
    std::string error;
    SymbolTable defaultTable;

    struct ExecutionContext : public std::enable_shared_from_this<ExecutionContext> {
        typedef std::shared_ptr<ExecutionContext> Ptr;
        typedef std::weak_ptr<ExecutionContext> WPtr;

        parser::Node::Ptr root;
        const std::string source;
        std::shared_ptr<std::thread> thread;
        script::SymbolTable table;
        std::atomic_bool running;

        ExecutionContext(parser::Node::Ptr root, const SymbolTable& table,
                         const std::string& source)
        : root(root)
        , source(source)
        , table(table)
        , running(true) {}
    };

    Script(const std::string& data, bool addDefaults);
    static std::optional<script::Value> execute(ExecutionContext::Ptr context);

    friend class Manager;
};

} // namespace script
} // namespace bl

#endif
