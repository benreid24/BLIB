#include <BLIB/Assets/Builtin/Animation2DPayload.hpp>

#include <SFML/Graphics/Transform.hpp>

namespace bl
{
namespace asi
{
namespace
{
bool compareVectors(const glm::vec2& left, const glm::vec2& right) {
    if (std::abs(left.x - right.x) >= 0.9f || std::abs(left.y - right.y) >= 0.9f) { return false; }
    return true;
}

void removeDuplicateShard(const Animation2DPayload::Frame& frame) {
    Animation2DPayload::Frame& f = const_cast<Animation2DPayload::Frame&>(frame);
    if (f.shards.size() == 2) {
        if (f.shards.front().source == f.shards.back().source &&
            f.shards.front().offset == f.shards.back().offset) {
            f.shards.pop_back();
        }
    }
}

void computeFrameSize(Animation2DPayload::Frame& frame) {
    sf::FloatRect bounds({1000000.f, 1000000.f}, {-1000000.f, -1000000.f});
    for (auto& shard : frame.shards) {
        const sf::FloatRect source(shard.source);
        const sf::Vector2f center(source.size.x * 0.5f, source.size.y * 0.5f);
        sf::Transform transform;
        transform.rotate(sf::degrees(shard.rotation), center);
        transform.scale({shard.scale.x, shard.scale.y}, center);
        transform.translate({shard.offset.x, shard.offset.y});

        const sf::FloatRect shardBounds = transform.transformRect(source);
        bounds.position.x               = std::min(bounds.position.x, shardBounds.position.x);
        bounds.position.y               = std::min(bounds.position.y, shardBounds.position.y);
        bounds.size.x = std::max(bounds.size.x, shardBounds.position.x + shardBounds.size.x);
        bounds.size.y = std::max(bounds.size.y, shardBounds.position.y + shardBounds.size.y);
    }

    frame.size = {bounds.size.x - bounds.position.x, bounds.size.y - bounds.position.y};
}

} // namespace

Animation2DPayload::Animation2DPayload(const as::Payload::ConstructContext& ctx)
: Payload(ctx)
, spritesheet(ctx.repo, *this, "spritesheet")
, frames()
, loop(false)
, centerShards(false) {}

void Animation2DPayload::computeDerivedData() {
    totalLength              = 0.f;
    std::uint32_t shardIndex = 0;
    const sf::Vector2f size(spritesheet->get().getSize());

    for (unsigned int i = 0; i < frames.size(); ++i) {
        auto& frame      = frames[i];
        frame.shardIndex = shardIndex;
        shardIndex += frame.shards.size();
        computeFrameSize(frame);
        totalLength += frame.length;

        for (auto& shard : frame.shards) {
            const sf::FloatRect source(shard.source);
            shard.normalizedSource.position.x = source.position.x / size.x;
            shard.normalizedSource.position.y = source.position.y / size.y;
            shard.normalizedSource.size.x     = source.size.x / size.x;
            shard.normalizedSource.size.y     = source.size.y / size.y;
        }
    }

    slideshow = isValidSlideshow();
}

bool Animation2DPayload::isValidSlideshow() {
    if (frames.empty()) return true;

    for (const auto& frame : frames) {
        // TODO - remove ugly hack when animation editor rewritten
        removeDuplicateShard(frame);

        if (frame.shards.size() != 1) {
            BL_LOG_DEBUG << "Slideshow check fail: Frame has more than 1 shard";
            return false;
        }
        if (frame.shards.front().rotation != 0.f) {
            BL_LOG_DEBUG << "Slideshow check fail: Frame has non-zero rotation";
            return false;
        }
    }

    const glm::vec2 pos   = frames.front().shards.front().offset;
    const glm::vec2 scale = frames.front().shards.front().scale;
    for (const auto& frame : frames) {
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

} // namespace asi
} // namespace bl
