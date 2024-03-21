#include <BLIB/Resources/ResourceManager.hpp>

#include <BLIB/Resources/GarbageCollector.hpp>

namespace bl
{
namespace resource
{
ResourceManagerBase::ResourceManagerBase()
: gcPeriod(300) {
    GarbageCollector::get().registerManager(this);
}

void ResourceManagerBase::unregister() { GarbageCollector::get().unregisterManager(this); }

void ResourceManagerBase::setGCPeriod(unsigned int gc) {
    gcPeriod = gc;
    GarbageCollector::get().managerPeriodChanged(this);
}

} // namespace resource
} // namespace bl
