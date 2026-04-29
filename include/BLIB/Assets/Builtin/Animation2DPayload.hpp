#ifndef BLIB_ASSETS_BUILTIN_ANIMATION2DPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_ANIMATION2DPAYLOAD_HPP

#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Dependency.hpp>
#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Serialization.hpp>

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
            sf::Vector2f offset;
            sf::Vector2f scale;
            float rotation;
            std::uint8_t alpha;
        };

        std::vector<Shard> shards;
        float length;
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
    const ImagePayload& getSpritesheet() const { return spritesheet.get(); }

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
    bool isCenterShards() const { return centerShards; }

private:
    as::Dependency<ImagePayload> spritesheet;
    std::vector<Frame> frames;
    bool loop;
    bool centerShards;

    friend struct serial::SerializableObject<Animation2DPayload>;
    friend class Animation2DDriver;
};

} // namespace asi

namespace serial
{
template<>
struct SerializableObject<asi::Animation2DPayload> : public SerializableObjectBase {
    SerializableField<1, asi::Animation2DPayload, std::vector<asi::Animation2DPayload::Frame>>
        frames;
    SerializableField<2, asi::Animation2DPayload, bool> loop;
    SerializableField<3, asi::Animation2DPayload, bool> centerShards;

    SerializableObject()
    : SerializableObjectBase("Animation2D")
    , frames("frames", *this, &asi::Animation2DPayload::frames, SerializableFieldBase::Required{})
    , loop("loop", *this, &asi::Animation2DPayload::loop, SerializableFieldBase::Required{})
    , centerShards("centerShards", *this, &asi::Animation2DPayload::centerShards,
                   SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<asi::Animation2DPayload::Frame> : public SerializableObjectBase {
    SerializableField<1, asi::Animation2DPayload::Frame,
                      std::vector<asi::Animation2DPayload::Frame::Shard>>
        shards;
    SerializableField<2, asi::Animation2DPayload::Frame, float> length;

    SerializableObject()
    : SerializableObjectBase("Animation2DFrame")
    , shards("shards", *this, &asi::Animation2DPayload::Frame::shards,
             SerializableFieldBase::Required{})
    , length("length", *this, &asi::Animation2DPayload::Frame::length,
             SerializableFieldBase::Required{}) {}
};

template<>
struct SerializableObject<asi::Animation2DPayload::Frame::Shard> : public SerializableObjectBase {
    SerializableField<1, asi::Animation2DPayload::Frame::Shard, sf::IntRect> source;
    SerializableField<2, asi::Animation2DPayload::Frame::Shard, sf::Vector2f> offset;
    SerializableField<3, asi::Animation2DPayload::Frame::Shard, sf::Vector2f> scale;
    SerializableField<4, asi::Animation2DPayload::Frame::Shard, float> rotation;
    SerializableField<5, asi::Animation2DPayload::Frame::Shard, std::uint8_t> alpha;

    SerializableObject()
    : SerializableObjectBase("Animation2DFrameShard")
    , source("source", *this, &asi::Animation2DPayload::Frame::Shard::source,
             SerializableFieldBase::Required{})
    , offset("offset", *this, &asi::Animation2DPayload::Frame::Shard::offset,
             SerializableFieldBase::Required{})
    , scale("scale", *this, &asi::Animation2DPayload::Frame::Shard::scale,
            SerializableFieldBase::Required{})
    , rotation("rotation", *this, &asi::Animation2DPayload::Frame::Shard::rotation,
               SerializableFieldBase::Required{})
    , alpha("alpha", *this, &asi::Animation2DPayload::Frame::Shard::alpha,
            SerializableFieldBase::Required{}) {}
};

} // namespace serial

} // namespace bl

#endif
