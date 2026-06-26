#ifndef BLIB_MODELS_ANIMATION_HPP
#define BLIB_MODELS_ANIMATION_HPP

#include <BLIB/Models/BoneAnimation.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>
#include <assimp/anim.h>
#include <string>
#include <vector>

namespace bl
{
namespace mdl
{
class BoneSet;
class NodeSet;

/**
 * @brief Representa an animation of a model
 *
 * @ingroup Models
 */
class Animation {
public:
    /**
     * @brief Creates an empty animation
     */
    Animation();

    /**
     * @brief Populates the animation from Assimp data
     *
     * @param src The source data
     * @param nodes The set of nodes in the model
     * @param bones The bones in the model
     */
    void populate(const aiAnimation* src, const NodeSet& nodes, const BoneSet& bones);

    /**
     * @brief Returns the name of the animation
     */
    const std::string& getName() const { return name; }

    /**
     * @brief Returns the duration of the animation in ticks
     */
    double getDurationInTicks() const { return durationInTicks; }

    /**
     * @brief Returns the ticks per second of the animation
     */
    double getTicksPerSecond() const { return ticksPerSecond; }

    /**
     * @brief Returns the bone animations in this animation. The indices of the channels here are
     *        the same as the bone indices of the model
     */
    const std::vector<BoneAnimation>& getBoneAnimations() const { return boneAnimations; }

    /**
     * @brief Initializes the animation with the given metadata
     *
     * @param name The name of the animation
     * @param durationInTicks The duration of the animation in ticks
     * @param ticksPerSecond The number of ticks per second
     */
    void init(const std::string& name, double durationInTicks, double ticksPerSecond);

    /**
     * @brief Adds a bone animation channel to this animation
     *
     * @param boneAnim The bone animation to add
     */
    void addBoneAnimation(BoneAnimation boneAnim);

private:
    std::string name;
    double durationInTicks;
    double ticksPerSecond;
    std::vector<BoneAnimation> boneAnimations;

    friend struct serial::SerializableObject<Animation>;
    friend struct refl::ReflectedObject<Animation>;
};

} // namespace mdl

namespace serial
{
template<>
struct SerializableObject<mdl::Animation> : public SerializableObjectBase {
    SerializableField<1, mdl::Animation, std::string> name;
    SerializableField<2, mdl::Animation, double> durationInTicks;
    SerializableField<3, mdl::Animation, double> ticksPerSecond;
    SerializableField<4, mdl::Animation, std::vector<mdl::BoneAnimation>> boneAnimations;

    SerializableObject()
    : SerializableObjectBase("Animation")
    , name("name", *this, &mdl::Animation::name, SerializableFieldBase::Required{})
    , durationInTicks("durationInTicks", *this, &mdl::Animation::durationInTicks,
                      SerializableFieldBase::Required{})
    , ticksPerSecond("ticksPerSecond", *this, &mdl::Animation::ticksPerSecond,
                     SerializableFieldBase::Required{})
    , boneAnimations("boneAnimations", *this, &mdl::Animation::boneAnimations,
                     SerializableFieldBase::Required{}) {}
};

} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<mdl::Animation> {
    inline static const auto spec = makeSpec<mdl::Animation>(
        "Animation",
        memberList(defineMember(1, "name", &mdl::Animation::name),
                   defineMember(2, "durationInTicks", &mdl::Animation::durationInTicks),
                   defineMember(3, "ticksPerSecond", &mdl::Animation::ticksPerSecond),
                   defineMember(4, "boneAnimations", &mdl::Animation::boneAnimations)));
};
} // namespace refl

} // namespace bl

#endif
