#include <BLIB/Engine/Systems.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace engine
{
Systems::Systems(Engine& e)
: engine(e) {
    for (auto& bucket : systems) { bucket.reserve(8); }
}

void Systems::init() {
    // TODO - order init differently in case of dependencies?
    for (auto& bucket : systems) {
        for (auto& system : bucket) { system.second->init(engine); }
    }
}

void Systems::update(FrameStage::V startStage, FrameStage::V endStage, StateMask::V stateMask,
                     float dt) {
    const auto beg = systems.begin() + startStage;
    const auto end = systems.begin() + endStage;

    for (auto it = beg; it != end; ++it) {
        // TODO - parallelize system updates within the same bucket
        for (auto& system : *it) {
            if ((system.first & stateMask) != 0) { system.second->update(dt); }
        }
    }
}

} // namespace engine
} // namespace bl
