#include <BLIB/Scripts/Manager.hpp>

#include <BLIB/Logging.hpp>
#include <chrono>

namespace bl
{
namespace script
{
Manager::~Manager() {
    BL_LOG_INFO << "ScriptManager (" << this << ") terminating with timeout = 2s";
    terminateAll(2.f);
    BL_LOG_INFO << "ScriptManager (" << this << ") terminated";
}

void Manager::watch(Script::ExecutionContext::WPtr record) {
    std::lock_guard guard(mutex);
    clean();
    scripts.push_back(record);
}

bool Manager::terminateAll(float timeout) {
    std::lock_guard guard(mutex);
    for (auto i = scripts.begin(); i != scripts.end(); ++i) {
        if (i->expired())
            i = scripts.erase(i);
        else {
            auto ptr = i->lock();
            if (ptr)
                ptr->table.kill();
            else
                i = scripts.erase(i);
        }
        if (i == scripts.end()) break; // break before ++i
    }
    const int ms = static_cast<int>(timeout * 1000.0f);
    for (int t = 0; t < ms; t += 30) {
        clean();
        if (scripts.empty()) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    if (!scripts.empty()) {
        BL_LOG_WARN << scripts.size() << "scripts still running after " << timeout << "s  timeout";
        return false;
    }
    return true;
}

void Manager::clean() {
    for (auto i = scripts.begin(); i != scripts.end(); ++i) {
        if (i->expired())
            i = scripts.erase(i);
        else {
            auto ptr = i->lock();
            if (!ptr || !ptr->running) i = scripts.erase(i);
        }
        if (i == scripts.end()) break; // break before ++i
    }
}

} // namespace script
} // namespace bl
