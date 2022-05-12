#include <BLIB/Resources/Manager.hpp>

#include <BLIB/Resources/GarbageCollector.hpp>

namespace bl
{
namespace resource
{
ManagerBase::ManagerBase(unsigned int gcPeriod)
: gcPeriod(gcPeriod) {
    GarbageCollector::get().registerManager(this);
}

void ManagerBase::unregister() { GarbageCollector::get().unregisterManager(this); }

} // namespace resource
} // namespace bl
