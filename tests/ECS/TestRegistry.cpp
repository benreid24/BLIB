#include "TestRegistry.hpp"
#include <BLIB/Engine.hpp>

namespace bl
{
namespace ecs
{
namespace unittest
{
engine::Engine engine(engine::Settings().withMaxEntityCount(MaxEntities));
Registry& testRegistry = engine.ecs();
}
}
}
