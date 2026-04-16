#include <BLIB/Assets/Drivers/SoundDriver.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <SFML/Audio/OutputSoundFile.hpp>

namespace bl
{
namespace asi
{
namespace
{
constexpr std::string_view TempName = "asset_sound_temp.wav";
}

bool SoundDriver::doCreate(const as::CreateContext& ctx, SoundPayload& payload) {
    if (!ctx.getCustomData().getPath().empty()) {
        return payload.get().loadFromFile(ctx.getCustomData().getPath());
    }
    return false;
}

bool SoundDriver::doRead(const as::ReadContext& ctx, SoundPayload& payload) {
    stream::InputStream input;
    if (!ctx.setupReadStream("sound.wav", input)) { return false; }
    stream::SfInputStreamAdaptor adaptor(input);
    return payload.get().loadFromStream(adaptor);
}

bool SoundDriver::doWrite(const as::WriteContext& ctx, const SoundPayload& payload) {
    stream::OutputStream output;
    if (!ctx.setupWriteStream("sound.wav", output)) { return false; }

    // first we need to write to a temp file due to sfml not allowing us to write to a custom stream
    const std::string tempFilename = util::FileUtil::genTempNameInTempDir("wav");
    sf::OutputSoundFile soundFile;
    if (!soundFile.openFromFile(tempFilename,
                                payload.get().getSampleRate(),
                                payload.get().getChannelCount(),
                                payload.get().getChannelMap())) {
        BL_LOG_ERROR << "Failed to open temp sound file for writing";
        return false;
    }
    soundFile.write(payload.get().getSamples(),
                    payload.get().getSampleCount() * payload.get().getChannelCount());
    soundFile.close();

    // now we can write the data from the temp file to the output stream
    std::ifstream tempInput(tempFilename, std::ios::binary);
    if (!tempInput) {
        BL_LOG_ERROR << "Failed to open temp sound file for reading";
        return false;
    }
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(1024 * 1024);
    while (tempInput) {
        tempInput.read(buffer.get(), 1024 * 1024);
        if (!output.write(buffer.get(), tempInput.gcount())) {
            BL_LOG_ERROR << "Failed to write temp sound file to output stream";
            return false;
        }
    }

    // cleanup but dont fail if it doesnt work
    util::FileUtil::deleteFile(tempFilename);

    return true;
}

} // namespace asi
} // namespace bl
