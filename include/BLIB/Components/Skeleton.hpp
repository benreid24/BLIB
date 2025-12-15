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
class Skeleton {
public:
    /**
     * @brief A single node in a skeleton. May or may not correspond to a bone
     *
     * @ingroup Components
     */
    struct Node {
        std::vector<Node*> children;
        Node* parent;

        std::optional<std::uint32_t> boneIndex;
        glm::mat4 boneOffset;
        glm::mat4 nodeLocalTransform;

        /**
         * @brief Initializes with sane defaults
         */
        Node();

        /**
         * @brief Initializes from the given model data
         *
         * @param model The model the bone belongs to
         * @param node The node of this specific bone
         */
        void init(const mdl::Model& model, const mdl::Node& node);
    };

    Node* root;
    std::uint32_t numBones;
    std::vector<Node> nodes;
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
};

} // namespace com
} // namespace bl

#endif
