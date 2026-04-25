#include <BLIB/Assets/Drivers/ImageDriver.hpp>

namespace bl
{
namespace asi
{
bool ImageDriver::doCreate(const as::CreateContext& ctx, ImagePayload& payload) {
    if (!ctx.getCustomData().getPath().empty()) {
        return payload.get().loadFromFile(ctx.getCustomData().getPath());
    }

    const CreateParams* params = ctx.getCustomDataAsMaybe<CreateParams>();
    if (params) {
        if (params->sourceImage) {
            payload.get().resize(
                {params->sourceImage->getSize().x, params->sourceImage->getSize().y});
            return payload.get().copy(*params->sourceImage, {});
        }
        else { payload.get().resize({params->width, params->height}, params->fillColor); }
    }
    return true;
}

bool ImageDriver::doRead(const as::ReadContext& ctx, ImagePayload& payload) {
    stream::InputStream input;
    if (!ctx.setupReadStream("image.png", input)) { return false; }
    stream::SfInputStreamAdaptor adaptor(input);
    return payload.get().loadFromStream(adaptor);
}

bool ImageDriver::doWrite(const as::WriteContext& ctx, const ImagePayload& payload) {
    stream::OutputStream output;
    if (!ctx.setupWriteStream("image.png", output)) { return false; }
    const auto data = payload.get().saveToMemory("png");
    if (!data.has_value()) { return false; }
    return output.write(data->data(), data->size());
}

} // namespace asi
} // namespace bl
