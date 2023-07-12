#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>

#include <limits>

namespace bl
{
namespace rc
{
namespace ds
{
DescriptorSetInstance::DescriptorSetInstance(BindMode bindMode, SpeedBucketSetting speedSetting)
: bindless(bindMode == Bindless)
, speedBind(speedSetting == RebindForNewSpeed) {}

} // namespace ds
} // namespace rc
} // namespace bl
