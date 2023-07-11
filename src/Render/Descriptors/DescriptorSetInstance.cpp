#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>

#include <limits>

namespace bl
{
namespace gfx
{
namespace ds
{
DescriptorSetInstance::DescriptorSetInstance(BindMode bindMode, SpeedBucketSetting speedSetting)
: bindless(bindMode == Bindless)
, speedBind(speedSetting == RebindForNewSpeed) {}

} // namespace ds
} // namespace gfx
} // namespace bl
