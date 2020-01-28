#ifndef BLIB_SCRIPTS_SCRIPT_HPP
#define BLIB_SCRIPTS_SCRIPT_HPP

#include <BLIB/Scripts/Parser.hpp>
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

private:
    parser::Node::Ptr root;

    /**
     * @brief Actually runs the script
     *
     */
    void execute();

    struct ExecutionRecord {
        typedef std::shared_ptr<ExecutionRecord> Ptr;
        typedef std::weak_ptr<ExecutionRecord> WPtr;

        std::thread thread;
        std::atomic_bool killed;

        ExecutionRecord()
        : killed(false) {}
        ExecutionRecord(Script* scr)
        : killed(false)
        , thread(&Script::execute, scr) {}
    };

    friend class ScriptManager;
};

} // namespace bl

#endif