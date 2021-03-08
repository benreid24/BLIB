#ifndef BLIB_SCRIPTS_SCRIPT_HPP
#define BLIB_SCRIPTS_SCRIPT_HPP

#include <BLIB/Parser/Node.hpp>
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
     * @param data
     */
    Script(const std::string& data);

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
     * @brief Runs the script in the current thread
     *
     * @param engine The Engine to use to manage spawned Scripts
     * @return Return value if completed successfully (void if no return) or null on error
     *
     */
    std::optional<script::Value> run(engine::Engine& engine) const;

    /**
     * @brief Runs the script in the background
     *
     * @param manager Engine to register scripts with
     *
     */
    void runBackground(engine::Engine& engine) const;

    /**
     * @brief Runs the script in the background
     *
     * @param manager Manager to register scripts with. Optional
     *
     */
    void runBackground(Manager* manager = nullptr) const;

protected:
    /**
     * @brief Allows specialized subclasses to populate the symbol table with custom functions
     *        to allow scripts to access whatever they are hooked into
     *
     */
    virtual void addCustomSymbols(script::SymbolTable& table) const {}

    /**
     * @brief Hook for custom Script classes to run custom code before running
     *
     */
    virtual void onRun() const {}

private:
    parser::Node::Ptr root;
    const std::string source;

    /**
     * @brief Calls generateCustomStartSymbols and adds built in methods
     *
     */
    script::SymbolTable generateBaseTable() const;

    struct ExecutionContext : public std::enable_shared_from_this<ExecutionContext> {
        typedef std::shared_ptr<ExecutionContext> Ptr;
        typedef std::weak_ptr<ExecutionContext> WPtr;

        parser::Node::Ptr root;
        std::shared_ptr<std::thread> thread;
        script::SymbolTable table;
        std::atomic_bool running;

        ExecutionContext(parser::Node::Ptr root)
        : root(root)
        , running(true) {}
    };

    /**
     * @brief Actually runs the script
     *
     */
    std::optional<script::Value> execute(ExecutionContext::Ptr context) const;

    friend class Manager;
};

} // namespace script
} // namespace bl

#endif
