#include <BLIB/Scripts/ScriptManager.hpp>

#include <chrono>
#include <iostream>

namespace bl
{
using namespace scripts;

void ScriptManager::watch(Script::ExecutionContext::WPtr record) {
    std::lock_guard guard(mutex);
    for (auto i = scripts.begin(); i != scripts.end(); ++i) {
        if (i->expired())
            i = scripts.erase(i);
        else {
            auto ptr = i->lock();
            if (ptr) {
                if (!ptr->running) i = scripts.erase(i);
            }
            else
                i = scripts.erase(i);
        }
    }
    scripts.push_back(record);
}

void ScriptManager::terminateAll(float timeout) {
    std::lock_guard guard(mutex);
    for (auto i = scripts.begin(); i != scripts.end(); ++i) {
        if (i->expired())
            i = scripts.erase(i);
        else {
            auto ptr = i->lock();
            if (ptr)
                ptr->killed = true;
            else
                i = scripts.erase(i);
        }
    }
    if (timeout > 0) {
        const int ms = static_cast<int>(timeout * 1000.0f);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    for (auto i = scripts.begin(); i != scripts.end(); ++i) {
        if (i->expired())
            i = scripts.erase(i);
        else {
            auto ptr = i->lock();
            if (!ptr || !ptr->running)
                i = scripts.erase(i);
            else
                std::cerr << "Script still running after " << timeout << "s  timeout"
                          << std::endl;
        }
    }
}

} // namespace bl