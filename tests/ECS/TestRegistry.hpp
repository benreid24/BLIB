#ifndef TESTS_ECS_TESTREGISTRY_HPP
#define TESTS_ECS_TESTREGISTRY_HPP

#include <BLIB/ECS/Registry.hpp>

namespace bl
{
namespace ecs
{
namespace unittest
{
constexpr unsigned int MaxEntities = 100;
extern Registry& testRegistry;
}
} // namespace ecs
} // namespace bl

#endif
