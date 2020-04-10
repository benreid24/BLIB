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
class ScriptManager;

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
     * @param manager The ScriptManager to use to manage spawned Scripts, if any
     * @return Return value if completed successfully (void if no return) or null on error
     *
     */
    std::optional<scripts::Value> run(ScriptManager* manager = nullptr) const;

    /**
     * @brief Runs the script in the background
     *
     * @param manager ScriptManager to register with, nullptr for none
     *
     */
    void runBackground(ScriptManager* manager = nullptr) const;

protected:
    /**
     * @brief Allows specialized subclasses to populate the symbol table with custom functions
     *        to allow scripts to access whatever they are hooked into
     *
     */
    virtual void addCustomSymbols(scripts::SymbolTable& table) const {}

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
    scripts::SymbolTable generateBaseTable() const;

    struct ExecutionContext : public std::enable_shared_from_this<ExecutionContext> {
        typedef std::shared_ptr<ExecutionContext> Ptr;
        typedef std::weak_ptr<ExecutionContext> WPtr;

        parser::Node::Ptr root;
        std::shared_ptr<std::thread> thread;
        scripts::SymbolTable table;
        std::atomic_bool running;

        ExecutionContext(parser::Node::Ptr root)
        : root(root)
        , running(true) {}
    };

    /**
     * @brief Actually runs the script
     *
     */
    std::optional<scripts::Value> execute(ExecutionContext::Ptr context) const;

    friend class ScriptManager;
};

} // namespace bl

#endif