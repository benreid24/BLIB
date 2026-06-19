#include <BLIB/Assets/Builtin/TexturePayload.hpp>

namespace bl
{
namespace asi
{
TexturePayload::CreateData::CreateData()
: as::CreateContext::CreateData()
, image()
, type(Type::Generic)
, colorSpace(ColorSpace::sRGB) {}

TexturePayload::CreateData::CreateData(const std::string& path)
: as::CreateContext::CreateData(path)
, type(Type::Generic)
, colorSpace(ColorSpace::sRGB) {}

TexturePayload::CreateData::CreateData(util::UUID image)
: CreateData() {
    this->image = image;
}

TexturePayload::TexturePayload(const Payload::ConstructContext& ctx)
: Payload(ctx)
, image(ctx.repo, *this, "image") {}

} // namespace asi
} // namespace bl
