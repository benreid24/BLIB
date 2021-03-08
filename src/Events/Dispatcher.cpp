#include <BLIB/Events/Dispatcher.hpp>

namespace bl
{
namespace event
{
void Dispatcher::remove(const std::type_index& t, void* val) {
    auto lit = listeners.find(t);
    if (lit != listeners.end()) {
        for (unsigned int j = 0; j < lit->second.size(); ++j) {
            if (lit->second[j] == val) {
                lit->second.erase(lit->second.begin() + j);
                --j;
            }
        }
    }
}

} // namespace event
} // namespace bl
