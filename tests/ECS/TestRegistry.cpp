#include "TestRegistry.hpp"
#include <BLIB/Engine.hpp>

namespace bl
{
namespace ecs
{
namespace unittest
{
// intentionally leak to avoid bad destruction (input system unsubscribe after dispatcher destructs)
engine::Engine* engine = new engine::Engine(engine::Settings().withMaxEntityCount(MaxEntities));
Registry& testRegistry = engine->ecs();
}
}
}
