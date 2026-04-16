#include <BLIB/Assets/Drivers/MusicDriver.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <SFML/Audio.hpp>

namespace bl
{
namespace asi
{
namespace
{
constexpr std::string_view TempName = "asset_sound_temp.wav";
}

bool MusicDriver::doCreate(const as::CreateContext& ctx, MusicPayload& payload) {
    if (ctx.getCustomData().getPath().empty()) { return false; }

    const std::string filepath = ctx.getFilePath("music.ogg");
    sf::InputSoundFile inputFile;
    if (!inputFile.openFromFile(ctx.getCustomData().getPath())) { return false; }

    sf::OutputSoundFile outputFile;
    if (!outputFile.openFromFile(filepath,
                                 inputFile.getSampleRate(),
                                 inputFile.getChannelCount(),
                                 inputFile.getChannelMap())) {
        BL_LOG_ERROR << "Failed to open output sound file for writing at path " << filepath;
        return false;
    }

    std::unique_ptr<std::int16_t[]> sampleBuffer = std::make_unique<std::int16_t[]>(8192);
    std::uint64_t totalSamplesRead               = 0;
    while (totalSamplesRead < inputFile.getSampleCount()) {
        std::uint64_t samplesToRead =
            std::min<std::uint64_t>(8192, inputFile.getSampleCount() - totalSamplesRead);
        const std::uint64_t samplesRead = inputFile.read(sampleBuffer.get(), samplesToRead);
        if (samplesRead == 0) { break; }
        outputFile.write(sampleBuffer.get(), samplesRead);
        totalSamplesRead += samplesRead;
    }
    inputFile.close();
    outputFile.close();

    return payload.get().openFromFile(filepath);
}

bool MusicDriver::doRead(const as::ReadContext& ctx, MusicPayload& payload) {
    // TODO - we need persistent streams
    return false;
}

bool MusicDriver::doWrite(const as::WriteContext&, const MusicPayload&) {
    // write is handled on create. sf::Music is read-only after create
    return true;
}

} // namespace asi
} // namespace bl
