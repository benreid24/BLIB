#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace engine
{
System::System(FrameStage::V frameStage, StateMask::V stateMask)
: frameStage(frameStage)
, stateMask(stateMask) {}

} // namespace engine
} // namespace bl
