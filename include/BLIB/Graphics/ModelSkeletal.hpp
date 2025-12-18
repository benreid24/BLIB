#ifndef BLIB_GRAPHICS_MODELSKELETAL_HPP
#define BLIB_GRAPHICS_MODELSKELETAL_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Components/Skeleton.hpp>
#include <BLIB/Components/SkeletonIndexLink.hpp>
#include <BLIB/Graphics/Components/Outline3D.hpp>
#include <BLIB/Graphics/Components/Transform3D.hpp>
#include <BLIB/Graphics/Drawable.hpp>
#include <BLIB/Models.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <BLIB/Resources/Ref.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Drawable for skeletal models with skinning and animation
 *
 * @ingroup Graphics
 */
class ModelSkeletal
: public Drawable<com::SkinnedMesh>
, public bcom::Transform3D
, public bcom::Outline3D {
public:
    /**
     * @brief Does nothing
     */
    ModelSkeletal();

    /**
     * @brief Creates the skeletal model from the given file
     *
     * @param world The world to create the model in
     * @param file The file to load the model from
     * @param materialPipelineId The material pipeline to render with
     * @return True if the model was able to loaded and created, false on error
     */
    bool create(engine::World& world, const std::string& file,
                std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3DSkinned);

    /**
     * @brief Creates the skeletal model from the given source model
     *
     * @param world The world to create the model in
     * @param model The model to render
     * @param materialPipelineId The material pipeline to render with
     * @return True if the model was able to created, false on error
     */
    bool create(engine::World& world, resource::Ref<mdl::Model> model,
                std::uint32_t materialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3DSkinned);

private:
    using Tx = ecs::Transaction<
        ecs::tx::EntityWrite, ecs::tx::ComponentRead<>,
        ecs::tx::ComponentWrite<com::Transform3D, com::SkinnedMesh, com::MaterialInstance,
                                com::Skeleton, com::SkeletonIndexLink>>;

    struct Child {
        ecs::Entity entity;
        com::SkinnedMesh* mesh;
        const void* src;
    };

    ecs::Registry* ecs;
    std::vector<Child> children;
    const void* selfMesh;
    bool createdMeshOnSelf;

    com::SkinnedMesh* createComponents(engine::World& world, Tx& tx, ecs::Entity entity,
                                       std::uint32_t materialPipelineId,
                                       const resource::Ref<mdl::Model>& model,
                                       const mdl::Mesh& src);
    void processNode(engine::World& world, Tx& tx, std::uint32_t materialPipelineId,
                     const resource::Ref<mdl::Model>& model, const mdl::Node& node);
    void createChild(engine::World& world, Tx& tx, std::uint32_t materialPipelineId,
                     const resource::Ref<mdl::Model>& model, const mdl::Mesh& src);

    virtual void onAdd(rc::Scene* scene, rc::UpdateSpeed updateFreq) override;
    virtual void onRemove() override;
    virtual void scaleToSize(const glm::vec2&) override {}
};

} // namespace gfx
} // namespace bl

#endif
