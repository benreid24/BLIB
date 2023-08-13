#include <BLIB/Graphics/Animation2D/AnimationData.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Serialization/Binary.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <cmath>

namespace bl
{
namespace gfx
{
namespace a2d
{
namespace
{
bool compareVectors(const sf::Vector2f& left, const sf::Vector2f& right) {
    if (std::abs(left.x - right.x) >= 0.9f || std::abs(left.y - right.y) >= 0.9f) { return false; }
    return true;
}
} // namespace

bool AnimationData::isValidSlideshow(const AnimationData& data) {
    if (data.frames.empty()) return true;

    for (const auto& frame : data.frames) {
        if (frame.shards.size() != 1) {
            BL_LOG_DEBUG << "Slideshow check fail: Frame has more than 1 shard";
            return false;
        }
        if (frame.shards.front().rotation != 0.f) {
            BL_LOG_DEBUG << "Slideshow check fail: Frame has non-zero rotation";
            return false;
        }
    }

    const sf::Vector2f pos   = data.frames.front().shards.front().offset;
    const sf::Vector2f scale = data.frames.front().shards.front().scale;
    for (const auto& frame : data.frames) {
        if (!compareVectors(scale, frame.shards.front().scale)) {
            BL_LOG_DEBUG << "Slideshow check fail: Frame scales inconsistent";
            return false;
        }
        if (!compareVectors(pos, frame.shards.front().offset)) {
            BL_LOG_DEBUG << "Slideshow check fail: Frame positions inconsistent";
            return false;
        }
    }

    return true;
}

AnimationData::AnimationData()
: totalLength(0)
, loop(false)
, centerShards(true) {}

bool AnimationData::loadFromFile(const std::string& filename) {
    serial::binary::InputFile file(filename);
    return doLoad(file, filename, false);
}

bool AnimationData::loadFromFileForBundling(const std::string& filename) {
    serial::binary::InputFile file(filename);
    return doLoad(file, filename, true);
}

bool AnimationData::saveToBundle(std::ostream& os) const {
    serial::StreamOutputBuffer wrapper(os);
    serial::binary::OutputStream output(wrapper);

    output.write(spritesheetSource);
    output.write(loop);
    output.write<std::uint16_t>(frames.size());
    for (unsigned int i = 0; i < frames.size(); ++i) {
        const Frame& frame = frames[i];
        output.write<std::uint32_t>(static_cast<std::uint32_t>(frame.length * 1000.f));
        output.write<std::uint16_t>(frame.shards.size());
        for (const Frame::Shard& shard : frame.shards) {
            output.write<std::uint32_t>(shard.source.left);
            output.write<std::uint32_t>(shard.source.top);
            output.write<std::uint32_t>(shard.source.width);
            output.write<std::uint32_t>(shard.source.height);
            output.write<std::uint32_t>(static_cast<std::uint32_t>(shard.scale.x * 100.f));
            output.write<std::uint32_t>(static_cast<std::uint32_t>(shard.scale.y * 100.f));
            output.write<std::int32_t>(shard.offset.x);
            output.write<std::int32_t>(shard.offset.y);
            output.write<std::uint32_t>(shard.rotation);
            output.write<std::uint8_t>(shard.alpha);
        }
    }

    return output.good();
}

bool AnimationData::loadFromMemory(const char* buffer, std::size_t len, const std::string& path) {
    serial::MemoryInputBuffer buf(buffer, len);
    serial::binary::InputStream stream(buf);
    return doLoad(stream, path, false);
}

bool AnimationData::doLoad(serial::binary::InputStream& input, const std::string& ogPath,
                           bool forBundle) {
    totalLength = 0.f;
    frames.clear();
    stateOffsets.clear();

    stateOffsets.reserve(16);
    stateOffsets.emplace_back(0);

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
        actualSpritesheetPath = sheet;
    }
    if (!input.read(loop)) return false;

    std::uint16_t nFrames = 0;
    if (!input.read(nFrames)) return false;
    frames.reserve(nFrames);
    for (unsigned int i = 0; i < nFrames; ++i) {
        Frame& frame = frames.emplace_back();

        std::uint32_t length;
        if (!input.read(length)) return false;
        frame.length = static_cast<float>(length) / 1000.0f;

        std::uint16_t nShards = 0;
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
            shard.offset.x = s32;
            if (!input.read(s32)) return false;
            shard.offset.y = s32;
            if (!input.read(u32)) return false;
            shard.rotation = u32;
            if (!input.read(shard.alpha)) return false;
            frame.shards.push_back(shard);
        }
        totalLength += frame.length;
        frame.nextFrame = i + 1;
        computeFrameSize(frame);
    }
    frames.back().nextFrame = 0;

    // bool value not present in legacy files
    std::uint8_t u8;
    if (!input.read(u8)) { centerShards = false; }
    else { centerShards = u8 == 1; }

    return true;
}

const std::string& AnimationData::spritesheetFile() const { return spritesheetSource; }

bool AnimationData::isLooping() const { return loop; }

float AnimationData::getLength() const { return totalLength; }

void AnimationData::computeFrameSize(Frame& frame) {
    sf::FloatRect bounds(1000000.f, 1000000.f, -1000000.f, -1000000.f);
    for (auto& shard : frame.shards) {
        const sf::FloatRect source(shard.source);
        const sf::Vector2f center(source.width * 0.5f, source.height * 0.5f);
        sf::Transform transform;
        transform.rotate(shard.rotation, center);
        transform.scale(shard.scale, center);
        transform.translate(shard.offset);

        const sf::FloatRect shardBounds = transform.transformRect(source);
        bounds.left                     = std::min(bounds.left, shardBounds.left);
        bounds.top                      = std::min(bounds.top, shardBounds.top);
        bounds.width  = std::max(bounds.width, shardBounds.left + shardBounds.width);
        bounds.height = std::max(bounds.height, shardBounds.top + shardBounds.height);
    }

    frame.size = {bounds.width - bounds.left, bounds.height - bounds.top};
}

sf::Vector2f AnimationData::getMaxSize() const {
    sf::Vector2f max(0, 0);
    for (unsigned int i = 0; i < frames.size(); ++i) {
        const sf::Vector2f s = getFrameSize(i);
        max.x                = std::max(max.x, s.x);
        max.y                = std::max(max.y, s.y);
    }
    return max;
}

void AnimationData::prepareForStates(std::size_t stateCount) { stateOffsets.resize(stateCount, 0); }

bool AnimationData::addState(std::size_t stateIndex, const AnimationData& anim, bool ss) {
    // validate compatible
    if (!frames.empty()) {
        if (anim.frames.empty()) {
            BL_LOG_ERROR << "Animation combine failed: State animation is empty";
            return false;
        }

        if (spritesheetSource != anim.spritesheetSource) {
            BL_LOG_ERROR << "Animation combine failed: Spritesheets are different";
            return false;
        }

        for (auto& frame : frames) {
            if (frame.shards.size() != frames.front().shards.size()) {
                BL_LOG_ERROR
                    << "Animation combine failed: Base animation has inconsistent shard counts";
                return false;
            }
        }

        for (const auto& frame : anim.frames) {
            if (frame.shards.size() != frames.front().shards.size()) {
                BL_LOG_ERROR << "Animation combine failed: State animation has different shard "
                                "counts than base";
                return false;
            }
        }

        if (ss) {
            if (isValidSlideshow(*this)) {
                if (!isValidSlideshow(anim)) {
                    BL_LOG_ERROR << "Animation combine failed: State animation was not Slideshow";
                    return false;
                }
            }
        }
    }

    // copy in frames and update next frame pointers
    const std::size_t offset = frames.size();
    stateOffsets[stateIndex] = offset;
    frames.reserve(frames.size() + anim.frames.size());
    std::copy(anim.frames.begin(), anim.frames.end(), std::back_inserter(frames));
    for (std::size_t i = offset; i < frames.size() - 1; ++i) { frames[i].nextFrame = i + 1; }
    frames.back().nextFrame = offset;

    return true;
}

std::size_t AnimationData::getStateFromFrame(std::size_t i) const {
    for (unsigned int j = 0; j < stateOffsets.size(); ++j) {
        if (i < stateOffsets[j]) { return j; }
    }
    BL_LOG_ERROR << "Invalid frame for lookup: " << i << ", frame count = " << frames.size();
    return 0;
}

} // namespace a2d
} // namespace gfx
} // namespace bl
