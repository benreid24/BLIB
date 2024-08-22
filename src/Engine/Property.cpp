#include <BLIB/Engine/Property.hpp>

#include <BLIB/Engine/Properties.hpp>

namespace bl
{
namespace engine
{
namespace priv
{
PropertyBase::PropertyBase() { Properties::addProperty(this); }

PropertyBase::~PropertyBase() { Properties::removeProperty(this); }

} // namespace priv
} // namespace engine
} // namespace bl
