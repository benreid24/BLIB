#include <BLIB/Media/Graphics/AnimationData.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Files/Binary/File.hpp>
#include <BLIB/Files/Util.hpp>

namespace bl
{
AnimationData::AnimationData()
: spritesheet()
, frames()
, totalLength(0)
, loop(false) {}

AnimationData::AnimationData(const std::string& filename) { load(filename); }

bool AnimationData::load(const std::string& filename) {
    frames.clear();
    totalLength = 0;

    file::binary::File file(filename, file::binary::File::Read);
    const std::string path = file::Util::getPath(filename);
    const std::string& spritesheetDir =
        engine::Configuration::get<std::string>("blib.animation.spritesheet_path");

    std::string sheet;
    if (!file.read(sheet)) return false;
    if (!file::Util::exists(sheet)) {
        if (file::Util::exists(file::Util::joinPath(path, sheet)))
            sheet = file::Util::joinPath(path, sheet);
        else if (file::Util::exists(file::Util::joinPath(spritesheetDir, sheet)))
            sheet = file::Util::joinPath(spritesheetDir, sheet);
        else
            return false;
    }
    spritesheet = engine::Resources::textures().load(sheet).data;
    if (!file.read(loop)) return false;

    uint16_t nFrames = 0;
    if (!file.read(nFrames)) return false;
    frames.reserve(nFrames);
    for (unsigned int i = 0; i < nFrames; ++i) {
        Frame frame;

        uint32_t length;
        if (!file.read(length)) return false;
        frame.length = static_cast<float>(length) / 1000.0f;

        Frame::Shard shard;
        uint16_t nShards = 0;
        if (!file.read(nShards)) return false;
        frame.shards.reserve(nShards);

        for (unsigned int j = 0; j < nShards; ++j) {
            uint32_t u32;
            int32_t s32;
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
            if (!file.read(s32)) return false;
            shard.posOffset.x = s32;
            if (!file.read(s32)) return false;
            shard.posOffset.y = s32;
            if (!file.read(u32)) return false;
            shard.rotation = u32;
            if (!file.read(shard.alpha)) return false;
            frame.shards.push_back(shard);
        }
        frames.push_back(frame);
        totalLength += frame.length;
    }

    return true;
}

bool AnimationData::isLooping() const { return loop; }

float AnimationData::getLength() const { return totalLength; }

unsigned int AnimationData::frameCount() const { return frames.size(); }

sf::Vector2f AnimationData::getFrameSize(unsigned int i) const {
    if (i >= frames.size()) return {0, 0};

    const Frame& frame = frames[i];
    sf::FloatRect bounds(0, 0, 0, 0);
    sf::Sprite sprite(*spritesheet);
    for (unsigned int j = 0; j < frame.shards.size(); ++j) {
        frame.shards[j].apply(sprite);
        bounds.left = std::min(bounds.left, sprite.getGlobalBounds().left);
        bounds.top  = std::min(bounds.top, sprite.getGlobalBounds().top);
        bounds.width =
            std::max(bounds.width, sprite.getGlobalBounds().left + sprite.getGlobalBounds().width);
        bounds.height =
            std::max(bounds.height, sprite.getGlobalBounds().top + sprite.getGlobalBounds().height);
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

void AnimationData::render(sf::RenderTarget& target, sf::RenderStates states, float elapsedTime,
                           const sf::Vector2f& pos, const sf::Vector2f& scale, float rotation,
                           bool centerOnOrigin, bool loopOverride, bool canLoop) const {
    if (frames.empty()) return;

    const bool isLoop = loopOverride ? canLoop : loop;
    unsigned int i    = 0;

    if (isLoop || elapsedTime <= totalLength) {
        while (elapsedTime > totalLength) elapsedTime -= totalLength;
        while (elapsedTime > 0) {
            if (frames[i].length > elapsedTime) break;
            elapsedTime -= frames[i].length;
            if (i >= frames.size() - 1) break;
            i += 1;
        }
    }
    else
        i = frames.size() - 1;

    const Frame& frame = frames[i];
    sf::Sprite s(*spritesheet);
    for (unsigned int j = 0; j < frame.shards.size(); ++j) {
        frame.shards[j].apply(s, scale, rotation, centerOnOrigin);
        s.move(pos);
        target.draw(s, states);
    }
}

void AnimationData::Frame::Shard::apply(sf::Sprite& s, const sf::Vector2f& sc, float rot,
                                        bool co) const {
    const sf::Vector2f center(source.width / 2, source.height / 2);
    s.setRotation(0);
    s.setTextureRect(source);
    s.setOrigin(center);
    s.setPosition(posOffset + center); // TODO - transform the offset by the rotation
    s.setScale(scale);
    s.scale(sc);
    if (co) s.move(-s.getGlobalBounds().width / 2, -s.getGlobalBounds().height / 2);
    s.setRotation(rotation + rot);
    s.setColor(sf::Color(255, 255, 255, alpha));
}

} // namespace bl