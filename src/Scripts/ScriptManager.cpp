#include <BLIB/Scripts/ScriptManager.hpp>

#include <chrono>
#include <iostream>

namespace bl
{
using namespace scripts;

void ScriptManager::watch(Script::ExecutionContext::WPtr record) {
    std::lock_guard guard(mutex);
    clean();
    scripts.push_back(record);
}

bool ScriptManager::terminateAll(float timeout) {
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
    }
    const int ms = static_cast<int>(timeout * 1000.0f);
    for (int t = 0; t < ms; t += 30) {
        clean();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    if (!scripts.empty()) {
        std::cerr << scripts.size() << "scripts still running after " << timeout
                  << "s  timeout" << std::endl;
        return false;
    }
    return true;
}

void ScriptManager::clean() {
    for (auto i = scripts.begin(); i != scripts.end(); ++i) {
        if (i->expired())
            i = scripts.erase(i);
        else {
            auto ptr = i->lock();
            if (!ptr || !ptr->running) i = scripts.erase(i);
        }
    }
}

} // namespace bl
