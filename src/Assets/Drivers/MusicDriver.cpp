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

MusicDriver::MusicDriver()
: Driver(
      as::bdl::AssetBundleConfig{.affinity  = as::bdl::AssetBundleConfig::Affinity::Parent,
                                 .selection = as::bdl::AssetBundleConfig::Selection::NonRoot,
                                 .onMount = as::bdl::AssetBundleConfig::OnMount::WhenRequested}) {}

bool MusicDriver::doCreate(as::CreateContext& ctx, MusicPayload& payload) {
    if (ctx.getCustomData().getPath().empty()) {
        BL_LOG_ERROR << "Music assets require a file path to import from";
        return false;
    }
    if (ctx.getMode() != as::Mode::Editor) {
        BL_LOG_ERROR << "Music assets can only be created in editor mode";
        return false;
    }

    const std::string filepath = ctx.getFilePath("music.ogg");
    if (!util::FileUtil::createDirectory(util::FileUtil::getPath(filepath))) {
        BL_LOG_ERROR << "Failed to create directory for music asset at path " << filepath;
        return false;
    }

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

bool MusicDriver::doRead(as::ReadContext& ctx, MusicPayload& payload) {
    as::PersistentStream* stream = ctx.getPersistentStream("music.ogg");
    if (!stream) { return false; }
    if (!payload.get().openFromStream(stream->getSFMLAdaptor())) { return false; }
    return true;
}

bool MusicDriver::doWrite(as::WriteContext& ctx, const MusicPayload&) {
    stream::InputStream readStream;
    if (!readStream.open(ctx.getFilePath("music.ogg"))) {
        BL_LOG_ERROR << "Failed to open source music file";
        return false;
    }
    stream::OutputStream writeStream;
    if (!ctx.setupWriteStream("music.ogg", writeStream)) {
        BL_LOG_ERROR << "Failed to setup music write stream";
        return false;
    }
    constexpr std::size_t ChunkSize = 8192;
    const std::size_t fullChunks    = readStream.getSize() / ChunkSize;
    const std::size_t lastChunkSize = readStream.getSize() % ChunkSize;
    const std::size_t totalChunks   = fullChunks + (lastChunkSize > 0 ? 1 : 0);

    std::vector<char> buffer;
    buffer.resize(ChunkSize);
    for (std::size_t i = 0; i < totalChunks; ++i) {
        const std::size_t toRead = i < fullChunks ? ChunkSize : lastChunkSize;
        if (!readStream.read(buffer, toRead)) {
            BL_LOG_ERROR << "Failed to read music chunk";
            return false;
        }
        if (!writeStream.write(buffer.data(), toRead)) {
            BL_LOG_ERROR << "Failed to write music chunk";
            return false;
        }
    }
    return true;
}

} // namespace asi
} // namespace bl
