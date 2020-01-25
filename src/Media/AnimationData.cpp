#include <BLIB/Media/AnimationData.hpp>

#include <BLIB/Files/BinaryFile.hpp>
#include <BLIB/Files/FileUtil.hpp>

namespace bl
{
struct AnimationData::Frame {
    struct Shard {
        sf::IntRect source;
        sf::Vector2f posOffset;
        sf::Vector2f scale;
        float rotation;
        uint8_t alpha;
    };

    float length;
    std::vector<Shard> shards;

    static void applyShard(const Shard& shard, sf::Sprite& sprite);
};

AnimationData::AnimationData()
: spritesheet()
, frames()
, totalLength(0)
, loop(false) {}

AnimationData::AnimationData(const std::string& filename, const std::string& spritesheetDir) {
    load(filename, spritesheetDir);
}

bool AnimationData::load(const std::string& filename, const std::string& spritesheetDir) {
    frames.clear();
    totalLength = 0;

    BinaryFile file(filename, BinaryFile::Read);
    const std::string path = FileUtil::getPath(filename);

    std::string sheet;
    if (!file.read(sheet)) return false;
    if (!FileUtil::exists(sheet)) {
        if (FileUtil::exists(FileUtil::joinPath(path, sheet)))
            sheet = FileUtil::joinPath(path, sheet);
        else if (FileUtil::exists(FileUtil::joinPath(spritesheetDir, sheet)))
            sheet = FileUtil::joinPath(spritesheetDir, sheet);
        else
            return false;
    }
    if (!spritesheet.loadFromFile(sheet)) return false;
    if (!file.read(loop)) return false;

    uint16_t nFrames = 0;
    if (!file.read(nFrames)) return false;
    frames.reserve(nFrames);
    for (unsigned int i = 0; i < nFrames; ++i) {
        Frame frame;
        Frame::Shard shard;
        uint16_t nShards = 0;

        if (!file.read(nShards)) return false;
        uint32_t length;
        if (!file.read<uint32_t>(length)) return false;
        frame.length = static_cast<float>(length) / 1000.0f;

        frame.shards.reserve(nShards);
        for (unsigned int j = 0; j < nShards; ++j) {
            uint32_t u32;
            if (!file.read(u32)) return false;
            shard.source.left = u32;
            if (!file.read(u32)) return false;
            shard.source.top = u32;
            if (!file.read(u32)) return false;
            shard.source.width = u32;
            if (!file.read(u32)) return false;
            shard.source.height = u32;
            if (!file.read(u32)) return false;
            shard.scale.x = static_cast<float>(u32) / 100.0f;
            if (!file.read(u32)) return false;
            shard.scale.y = static_cast<float>(u32) / 100.0f;
            if (!file.read(u32)) return false;
            shard.posOffset.x = u32;
            if (!file.read(u32)) return false;
            shard.posOffset.y = u32;
            if (!file.read(u32)) return false;
            shard.rotation = u32;
            if (!file.read(shard.alpha)) return false;
            frame.shards.push_back(shard);
        }
        frames.push_back(std::shared_ptr<Frame>(new Frame(frame)));
        totalLength += frame.length;
    }

    return true;
}

bool AnimationData::isLooping() const { return loop; }

float AnimationData::getLength() const { return totalLength; }

unsigned int AnimationData::frameCount() const { return frames.size(); }

sf::Vector2f AnimationData::getFrameSize(unsigned int i) const {
    if (i >= frames.size()) return {0, 0};

    const Frame& frame = *frames[i];
    sf::FloatRect bounds(0, 0, 0, 0);
    sf::Sprite sprite(spritesheet);
    for (unsigned int j = 0; j < frame.shards.size(); ++j) {
        Frame::applyShard(frame.shards[j], sprite);
        bounds.left  = std::min(bounds.left, sprite.getGlobalBounds().left);
        bounds.top   = std::min(bounds.top, sprite.getGlobalBounds().top);
        bounds.width = std::max(
            bounds.width, sprite.getGlobalBounds().left + sprite.getGlobalBounds().width);
        bounds.height = std::max(
            bounds.height, sprite.getGlobalBounds().top + sprite.getGlobalBounds().height);
    }

    return {bounds.width - bounds.left, bounds.height - bounds.top};
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

void AnimationData::render(sf::RenderTarget& target, float elapsedTime,
                           const sf::Vector2f& pos, const sf::Vector2f& scale, float rotation,
                           bool centerOnOrigin, bool loopOverride, bool canLoop) const {
    if (frames.empty()) return;

    const bool isLoop = loopOverride ? canLoop : loop;
    unsigned int i    = 0;

    if (isLoop || elapsedTime <= totalLength) {
        while (elapsedTime > totalLength) elapsedTime -= totalLength;
        while (elapsedTime > 0) {
            if (frames[i]->length > elapsedTime) break;
            elapsedTime - frames[i]->length;
            if (i >= frames.size() - 1) break;
            i += 1;
        }
    }
    else
        i = frames.size() - 1;

    const Frame& frame = *frames[i];
    sf::Sprite s(spritesheet);
    for (unsigned int j = 0; j < frame.shards.size(); ++j) {
        Frame::applyShard(frame.shards[j], s);
        s.move(pos);
        s.rotate(rotation);
        s.scale(scale);
        if (centerOnOrigin)
            s.setOrigin(frame.shards[j].source.width / 2, frame.shards[j].source.height / 2);
        target.draw(s);
    }
}

void AnimationData::Frame::applyShard(const Shard& shard, sf::Sprite& s) {
    s.setTextureRect(shard.source);
    s.setPosition(shard.posOffset);
    s.setRotation(shard.rotation);
    s.setScale(shard.scale);
    s.setColor(sf::Color(255, 255, 255, shard.alpha));
}

} // namespace bl