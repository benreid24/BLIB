#include "TestRegistry.hpp"
#include <BLIB/Engine.hpp>

namespace bl
{
namespace ecs
{
namespace unittest
{
Registry& testRegistry() {
    // intentionally leak to avoid bad destruction (input system unsubscribe after dispatcher
    // destructs)
    static engine::Engine* engine =
        new engine::Engine(engine::Settings().withMaxEntityCount(MaxEntities));
    return engine->ecs();
}
}
}
}
