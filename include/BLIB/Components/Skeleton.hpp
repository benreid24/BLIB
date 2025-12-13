#ifndef BLIB_COMPONENTS_SKELETON_HPP
#define BLIB_COMPONENTS_SKELETON_HPP

#include <BLIB/Models/Model.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>

namespace bl
{
namespace com
{
/**
 * @brief A skeleton of bones for skeletal animation
 *
 * @ingroup Components
 */
class Skeleton : public rc::rcom::DescriptorComponentBase<Skeleton, glm::mat4*> {
public:
    /**
     * @brief A single bone in a skeleton
     *
     * @ingroup Components
     */
    struct Bone {
        std::vector<Bone*> children;
        Bone* parent;

        glm::mat4 boneOffset;
        glm::mat4 nodeBindPoseTransform;
        glm::mat4 activeTransform;

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
    virtual void refreshDescriptor(glm::mat4*& dst) override;
};

} // namespace com
} // namespace bl

#endif
