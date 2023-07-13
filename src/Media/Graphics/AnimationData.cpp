#include <BLIB/Media/Graphics/AnimationData.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Serialization/Binary.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <cmath>

namespace bl
{
namespace rc
{
AnimationData::AnimationData()
: spritesheet()
, frames()
, totalLength(0)
, loop(false)
, centerShards(true) {}

bool AnimationData::loadFromFile(const std::string& filename) {
    frames.clear();
    totalLength = 0;

    serial::binary::InputFile file(filename);
    return doLoad(file, filename, false);
}

bool AnimationData::loadFromFileForBundling(const std::string& filename) {
    frames.clear();
    totalLength = 0;

    serial::binary::InputFile file(filename);
    return doLoad(file, filename, true);
}

bool AnimationData::saveToBundle(std::ostream& os) const {
    serial::StreamOutputBuffer wrapper(os);
    bl::serial::binary::OutputStream output(wrapper);

    output.write(spritesheetSource);
    output.write(loop);
    output.write<std::uint16_t>(frames.size());
    for (unsigned int i = 0; i < frameData.size(); ++i) {
        const Frame& frame = frameData[i];
        output.write<std::uint32_t>(static_cast<std::uint32_t>(lengths[i] * 1000.f));
        output.write<std::uint16_t>(frame.shards.size());
        for (const Frame::Shard& shard : frame.shards) {
            output.write<std::uint32_t>(shard.source.left);
            output.write<std::uint32_t>(shard.source.top);
            output.write<std::uint32_t>(shard.source.width);
            output.write<std::uint32_t>(shard.source.height);
            output.write<std::uint32_t>(static_cast<std::uint32_t>(shard.scale.x * 100.f));
            output.write<std::uint32_t>(static_cast<std::uint32_t>(shard.scale.y * 100.f));
            output.write<std::int32_t>(shard.posOffset.x);
            output.write<std::int32_t>(shard.posOffset.y);
            output.write<std::uint32_t>(shard.rotation);
            output.write<std::uint8_t>(shard.alpha);
        }
    }

    return output.good();
}

bool AnimationData::loadFromMemory(const char* buffer, std::size_t len, const std::string& path) {
    frames.clear();
    totalLength = 0;

    serial::MemoryInputBuffer buf(buffer, len);
    serial::binary::InputStream stream(buf);
    return doLoad(stream, path, false);
}

bool AnimationData::doLoad(serial::binary::InputStream& input, const std::string& ogPath,
                           bool forBundle) {
    const std::string path = util::FileUtil::getPath(ogPath);
    const std::string& spritesheetDir =
        engine::Configuration::get<std::string>("blib.animation.spritesheet_path");

    std::string sheet;
    if (!input.read(sheet)) return false;
    spritesheetSource = sheet;
    if (!resource::FileSystem::resourceExists(sheet)) {
        const std::string localSheet = util::FileUtil::joinPath(path, sheet);
        const std::string dirSheet   = util::FileUtil::joinPath(spritesheetDir, sheet);

        if (resource::FileSystem::resourceExists(localSheet)) { sheet = localSheet; }
        else if (resource::FileSystem::resourceExists(dirSheet)) { sheet = dirSheet; }
        else { return false; }

        if (forBundle) { spritesheetSource = sheet; }
    }
    spritesheet = resource::ResourceManager<sf::Texture>::load(sheet);
    if (!input.read(loop)) return false;

    uint16_t nFrames = 0;
    if (!input.read(nFrames)) return false;
    frameData.reserve(nFrames);
    lengths.reserve(nFrames);
    for (unsigned int i = 0; i < nFrames; ++i) {
        frameData.emplace_back();
        Frame& frame = frameData.back();

        uint32_t length;
        if (!input.read(length)) return false;
        lengths.emplace_back(static_cast<float>(length) / 1000.0f);

        uint16_t nShards = 0;
        if (!input.read(nShards)) return false;
        frame.shards.reserve(nShards);

        for (unsigned int j = 0; j < nShards; ++j) {
            frame.shards.emplace_back();
            Frame::Shard& shard = frame.shards.back();

            uint32_t u32;
            int32_t s32;
            if (!input.read(u32)) return false;
            shard.source.left = u32;
            if (!input.read(u32)) return false;
            shard.source.top = u32;
            if (!input.read(u32)) return false;
            shard.source.width = u32;
            if (!input.read(u32)) return false;
            shard.source.height = u32;
            if (!input.read(u32)) return false;
            shard.scale.x = static_cast<float>(u32) / 100.0f;
            if (!input.read(u32)) return false;
            shard.scale.y = static_cast<float>(u32) / 100.0f;
            if (!input.read(s32)) return false;
            shard.posOffset.x = s32;
            if (!input.read(s32)) return false;
            shard.posOffset.y = s32;
            if (!input.read(u32)) return false;
            shard.rotation = u32;
            if (!input.read(shard.alpha)) return false;
            frame.shards.push_back(shard);
        }
        totalLength += lengths.back();
    }

    // bool value not present in legacy files
    std::uint8_t u8;
    if (!input.read(u8))
        centerShards = false;
    else
        centerShards = u8 == 1;

    frames.reserve(frameData.size());
    for (const Frame& frame : frameData) {
        frames.emplace_back(
            sf::PrimitiveType::Quads, sf::VertexBuffer::Usage::Static, frame.shards.size() * 4);
        unsigned int offset = 0;
        for (const Frame::Shard& shard : frame.shards) {
            shard.apply(frames.back(), offset, centerShards);
            offset += 4;
        }
        frames.back().update();
    }

    sizes.reserve(frames.size());
    for (unsigned int i = 0; i < frames.size(); ++i) { sizes.emplace_back(computeFrameSize(i)); }

    if (!forBundle) {
        frameData.clear();
        frameData.shrink_to_fit();
    }

    return true;
}

const std::string& AnimationData::spritesheetFile() const { return spritesheetSource; }

bool AnimationData::isLooping() const { return loop; }

float AnimationData::getLength() const { return totalLength; }

unsigned int AnimationData::frameCount() const { return frames.size(); }

sf::Vector2f AnimationData::computeFrameSize(unsigned int i) const {
    if (i >= frames.size()) return {0, 0};

    const VertexBuffer& frame = frames[i];
    sf::FloatRect bounds(1000000.f, 1000000.f, -1000000.f, -1000000.f);
    for (unsigned int i = 0; i < frame.size(); ++i) {
        const auto& pos = frame[i].position;
        bounds.left     = std::min(bounds.left, pos.x);
        bounds.top      = std::min(bounds.top, pos.y);
        bounds.width    = std::max(bounds.width, pos.x);
        bounds.height   = std::max(bounds.height, pos.y);
    }

    return {bounds.width - bounds.left, bounds.height - bounds.top};
}

const sf::Vector2f& AnimationData::getFrameSize(unsigned int i) const { return sizes[i]; }

sf::Vector2f AnimationData::getMaxSize() const {
    sf::Vector2f max(0, 0);
    for (unsigned int i = 0; i < frames.size(); ++i) {
        const sf::Vector2f s = getFrameSize(i);
        max.x                = std::max(max.x, s.x);
        max.y                = std::max(max.y, s.y);
    }
    return max;
}

void AnimationData::render(sf::RenderTarget& target, sf::RenderStates states,
                           const sf::Vector2f& pos, const sf::Vector2f& scale, float rotation,
                           unsigned int i) const {
    if (frames.empty()) return;

    const VertexBuffer& frame = frames[i];
    states.texture            = spritesheet.get();
    states.transform.translate(pos);
    states.transform.rotate(rotation);
    states.transform.scale(scale);
    target.draw(frame, states);
}

void AnimationData::Frame::Shard::apply(VertexBuffer& buffer, unsigned int offset,
                                        bool centerShards) const {
    for (unsigned int i = 0; i < 4; ++i) {
        buffer[offset + i].color = sf::Color(255, 255, 255, alpha);
    }
    const sf::FloatRect s(source);
    buffer[offset].texCoords     = {s.left, s.top};
    buffer[offset + 1].texCoords = {s.left + s.width, s.top};
    buffer[offset + 2].texCoords = {s.left + s.width, s.top + s.height};
    buffer[offset + 3].texCoords = {s.left, s.top + s.height};

    sf::Vector2f halfSize(s.width * 0.5f, s.height * 0.5f);
    halfSize.x *= scale.x;
    halfSize.y *= scale.y;
    const sf::Vector2f points[] = {{-halfSize.x, -halfSize.y},
                                   {halfSize.x, -halfSize.y},
                                   {halfSize.x, halfSize.y},
                                   {-halfSize.x, halfSize.y}};
    const float r               = rotation / 180.f * 3.1415926f;
    const float cosA            = std::cos(r);
    const float sinA            = std::sin(r);
    for (unsigned int i = 0; i < 4; ++i) {
        sf::Vertex& vertex = buffer[offset + i];
        vertex.position.x  = posOffset.x + points[i].x * cosA - points[i].y * sinA + s.width * 0.5f;
        vertex.position.y = posOffset.y + points[i].x * sinA + points[i].y * cosA + s.height * 0.5f;
        if (centerShards) {
            vertex.position.x -= halfSize.x;
            vertex.position.y -= halfSize.y;
        }
    }
}

} // namespace rc
} // namespace bl
