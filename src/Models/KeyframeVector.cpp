#include <BLIB/Models/KeyframeVector.hpp>

#include <BLIB/Models/ConversionHelpers.hpp>

namespace bl
{
namespace mdl
{
KeyframeVector::KeyframeVector()
: time(0.0)
, value(0.f)
, interpolation(Linear) {}

void KeyframeVector::populate(const aiVectorKey& src) {
    time          = src.mTime;
    value         = Convert::toVec3(src.mValue);
    interpolation = static_cast<Interpolation>(src.mInterpolation);
}

} // namespace mdl
} // namespace bl
