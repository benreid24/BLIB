#include <BLIB/Entities/Entity.hpp>
#include <BLIB/Util/Random.hpp>
#include <limits>

namespace bl
{
namespace entity
{
IdGenerator::IdGenerator()
: seq(false)
, nextEntity(1) {}

IdGenerator& IdGenerator::get() {
    static IdGenerator gen;
    return gen;
}

void IdGenerator::generateSequentialIds(bool seq) { get().seq = seq; }

Entity IdGenerator::makeNew() {
    if (get().seq) {
        get().idLock.lock();
        const Entity e = get().nextEntity++;
        get().idLock.unlock();
        return e;
    }
    return Random::get<Entity>(1, std::numeric_limits<Entity>::max());
}

} // namespace entity
} // namespace bl
