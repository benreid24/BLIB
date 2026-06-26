#ifndef BLIB_ASSETS_BUILTIN_ANIMATION2DPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_ANIMATION2DPAYLOAD_HPP

#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Dependency.hpp>
#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <SFML/System.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace asi
{
class Animation2DDriver;

/**
 * @brief Payload for a 2D animation asset
 *
 * @ingroup Assets
 */
class Animation2DPayload : public as::Payload {
public:
    /**
     * @brief Represents a frame in a 2d animation
     *
     * @ingroup Assets
     */
    struct Frame {
        /**
         * @brief Represents a sprite in a 2d animation frame
         *
         * @ingroup Assets
         */
        struct Shard {
            sf::IntRect source;
            glm::vec2 offset;
            glm::vec2 scale;
            float rotation;
            std::uint8_t alpha;

            // computed
            sf::FloatRect normalizedSource;
        };

        std::vector<Shard> shards;
        float length;

        // computed
        std::uint32_t shardIndex;
        glm::vec2 size;
    };

    /**
     * @brief Creates an empty animation payload
     *
     * @param ctx The context to construct with
     */
    Animation2DPayload(const as::Payload::ConstructContext& ctx);

    /**
     * @brief Destroys the payload
     */
    virtual ~Animation2DPayload() = default;

    /**
     * @brief Returns the spritesheet asset payload
     */
    as::TypedRef<ImagePayload> getSpritesheet() const { return spritesheet.getRef(); }

    /**
     * @brief Returns the UUID of the spritesheet asset
     */
    util::UUID getSpritesheetId() const { return spritesheet.getUUID(); }

    /**
     * @brief Returns the frames of the animation
     */
    const std::vector<Frame>& getFrames() const { return frames; }

    /**
     * @brief Returns whether the animation should loop
     */
    bool isLooping() const { return loop; }

    /**
     * @brief Returns whether shards are centered or if the origin is the local top left corner
     */
    bool shardsAreCentered() const { return centerShards; }

    /**
     * @brief Returns the total duration of the animation in seconds
     */
    float getDuration() const { return totalLength; }

    /**
     * @brief Returns whether or not the animation is a slideshow
     */
    bool isSlideshow() const { return slideshow; }

private:
    as::Dependency<ImagePayload> spritesheet;
    std::vector<Frame> frames;
    bool loop;
    bool centerShards;

    // derived
    float totalLength;
    bool slideshow;

    void computeDerivedData();
    bool isValidSlideshow();

    friend struct refl::ReflectedObject<Animation2DPayload>;
    friend class Animation2DDriver;
};

} // namespace asi

namespace refl
{
template<>
struct ReflectedObject<asi::Animation2DPayload> {
    inline static const auto spec = makeSpec<asi::Animation2DPayload>(
        "Animation2D",
        memberList(defineMember(1, "frames", &asi::Animation2DPayload::frames),
                   defineMember(2, "loop", &asi::Animation2DPayload::loop),
                   defineMember(3, "centerShards", &asi::Animation2DPayload::centerShards)));
};

template<>
struct ReflectedObject<asi::Animation2DPayload::Frame> {
    inline static const auto spec = makeSpec<asi::Animation2DPayload::Frame>(
        "Animation2DFrame",
        memberList(defineMember(1, "shards", &asi::Animation2DPayload::Frame::shards),
                   defineMember(2, "length", &asi::Animation2DPayload::Frame::length)));
};

template<>
struct ReflectedObject<asi::Animation2DPayload::Frame::Shard> {
    inline static const auto spec = makeSpec<asi::Animation2DPayload::Frame::Shard>(
        "Animation2DFrameShard",
        memberList(defineMember(1, "source", &asi::Animation2DPayload::Frame::Shard::source),
                   defineMember(2, "offset", &asi::Animation2DPayload::Frame::Shard::offset),
                   defineMember(3, "scale", &asi::Animation2DPayload::Frame::Shard::scale),
                   defineMember(4, "rotation", &asi::Animation2DPayload::Frame::Shard::rotation),
                   defineMember(5, "alpha", &asi::Animation2DPayload::Frame::Shard::alpha)));
};
} // namespace refl

} // namespace bl

#endif
