#include <BLIB/Resources/Manager.hpp>

#include <BLIB/Resources/GarbageCollector.hpp>

namespace bl
{
namespace resource
{
ManagerBase::ManagerBase(unsigned int gcPeriod)
: gcPeriod(gcPeriod) {
    GarbageCollector::registerManager(this);
}

ManagerBase::~ManagerBase() { GarbageCollector::unregisterManager(this); }

} // namespace resource
} // namespace bl
