#include <BLIB/Engine/Systems.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace engine
{
Systems::StageSet::StageSet() { systems.reserve(8); };

Systems::Systems(Engine& e)
: engine(e) {}

void Systems::init() {
    for (auto& set : systems) {
        for (auto& system : set.systems) { system.system->init(engine); }
    }
}

void Systems::update(FrameStage::V startStage, FrameStage::V endStage, StateMask::V stateMask,
                     float dt) {
    const auto beg = systems.begin() + startStage;
    const auto end = systems.begin() + endStage;

    for (auto it = beg; it != end; ++it) {
        // TODO - parallelize system updates within the same bucket
        for (auto& system : it->systems) {
            if ((system.mask & stateMask) != 0) { system.system->update(it->mutex, dt); }
        }
    }
}

} // namespace engine
} // namespace bl
