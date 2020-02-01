#ifndef BLIB_SCRIPTS_SCRIPT_HPP
#define BLIB_SCRIPTS_SCRIPT_HPP

#include <BLIB/Parser/Node.hpp>
#include <BLIB/Scripts/SymbolTable.hpp>

#include <atomic>
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
     * @param timeout How long to run for before terminating
     * @return True on no errors, false if error
     */
    bool run(float timeout);

    /**
     * @brief Runs the script in the background
     *
     */
    void run();

protected:
    /**
     * @brief Allows specialized subclasses to populate the symbol table with custom functions
     *        to allow scripts to access whatever they are hooked into
     *
     */
    virtual scripts::SymbolTable generateCustomStartSymbols() const;

    /**
     * @brief Hook for custom Script classes to run custom code before running
     *
     */
    virtual void onRun() const {}

private:
    parser::Node::Ptr root;

    /**
     * @brief Calls generateCustomStartSymbols and adds built in methods
     *
     */
    scripts::SymbolTable generateBaseTable() const;

    struct ExecutionContext : public std::enable_shared_from_this<ExecutionContext> {
        typedef std::shared_ptr<ExecutionContext> Ptr;
        typedef std::weak_ptr<ExecutionContext> WPtr;

        std::thread thread;
        std::atomic_bool killed;
        std::atomic_bool running;

        ExecutionContext()
        : killed(false) {}
        ExecutionContext(Script* scr)
        : killed(false)
        , thread(&Script::execute, scr, shared_from_this()) {}
    };

    /**
     * @brief Actually runs the script
     *
     */
    void execute(ExecutionContext::Ptr context);

    friend class ScriptManager;
};

} // namespace bl

#endif