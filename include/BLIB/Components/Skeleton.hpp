#ifndef BLIB_COMPONENTS_SKELETON_HPP
#define BLIB_COMPONENTS_SKELETON_HPP

#include <BLIB/Models/Model.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <BLIB/Render/ShaderResources/SkeletalBonesResource.hpp>

namespace bl
{
namespace com
{
/**
 * @brief A skeleton of bones for skeletal animation
 *
 * @ingroup Components
 */
class Skeleton : public rc::rcom::DescriptorComponentBase<Skeleton, std::uint32_t> {
public:
    /**
     * @brief A single bone in a skeleton
     *
     * @ingroup Components
     */
    struct Bone {
        std::vector<Bone*> children;
        Bone* parent;

        std::uint32_t localIndex;
        glm::mat4 boneOffset;
        glm::mat4 nodeBindPoseTransform;

        /**
         * @brief Initializes with sane defaults
         */
        Bone();

        /**
         * @brief Initializes from the given model data
         *
         * @param model The model the bone belongs to
         * @param node The node of this specific bone
         */
        void init(const mdl::Model& model, const mdl::Node& node);
    };

    Bone* root;
    std::vector<Bone> bones;
    rc::sri::SkeletalBonesResource::ComponentLink resourceLink;
    bool needsRefresh;

    /**
     * @brief Initializes with sane defaults
     */
    Skeleton() = default;

    /**
     * @brief Initializes from the given model
     *
     * @param model The model data to initialize from
     */
    void init(const mdl::Model& model);

    /**
     * @brief Updates the active transforms of all bones in the skeleton
     *
     * @param dst The output transform array
     */
    virtual void refreshDescriptor(std::uint32_t& offset) override;

    /**
     * @brief Marks the bone offset index dirty for copy into the uniform buffer
     */
    void markForOffsetCopy() { markDirty(); }
};

} // namespace com
} // namespace bl

#endif
