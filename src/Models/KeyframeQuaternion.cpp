#include <BLIB/Models/KeyframeQuaternion.hpp>

namespace bl
{
namespace mdl
{
KeyframeQuaternion::KeyframeQuaternion()
: time(0.0)
, value(1.f, 0.f, 0.f, 0.f)
, interpolation(Interpolation::Linear) {}

void KeyframeQuaternion::populate(const aiQuatKey& src) {
    time          = static_cast<double>(src.mTime);
    value         = glm::quat(src.mValue.w, src.mValue.x, src.mValue.y, src.mValue.z);
    interpolation = static_cast<Interpolation>(src.mInterpolation);
}

} // namespace mdl
} // namespace bl
