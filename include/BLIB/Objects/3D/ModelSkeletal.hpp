#ifndef BLIB_OBJECTS_3D_MODELSKELETAL_HPP
#define BLIB_OBJECTS_3D_MODELSKELETAL_HPP

#include <BLIB/Assets/Builtin/ModelPayload.hpp>
#include <BLIB/Assets/TypedRef.hpp>
#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Components/Skeleton.hpp>
#include <BLIB/Components/SkeletonIndexLink.hpp>
#include <BLIB/Models.hpp>
#include <BLIB/Objects/3D/DrawableObject3D.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <variant>

namespace bl
{
namespace obj
{
namespace o3d
{
/**
 * @brief Drawable for skeletal models with skinning and animation
 *
 * @ingroup Objects
 */
class ModelSkeletal : public DrawableObject3D<com::SkinnedMesh> {
public:
    /**
     * @brief Does nothing
     */
    ModelSkeletal();

    /**
     * @brief Returns the skeleton of the model in the ECS. Only call after create()
     */
    com::Skeleton& getSkeleton() { return *skeleton; }

    /**
     * @brief Creates the skeletal model from the given file
     *
     * @param world The world to create the model in
     * @param file The file to load the model from
     * @param skinnedMeshMaterialPipelineId The material pipeline to render skinned meshes with
     * @param unskinnedMeshMaterialPipelineId The material pipeline to render unskinned meshes with
     * @return True if the model was able to loaded and created, false on error
     */
    bool create(
        engine::World& world, const std::string& file,
        std::uint32_t skinnedMeshMaterialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3DSkinned,
        std::uint32_t unskinnedMeshMaterialPipelineId =
            rc::cfg::MaterialPipelineIds::Mesh3DMaterial);

    /**
     * @brief Creates the skeletal model from the given source model
     *
     * @param world The world to create the model in
     * @param model The model to render
     * @param skinnedMeshMaterialPipelineId The material pipeline to render skinned meshes with
     * @param unskinnedMeshMaterialPipelineId The material pipeline to render unskinned meshes with
     * @return True if the model was able to created, false on error
     */
    bool create(
        engine::World& world, as::TypedRef<asi::ModelPayload> model,
        std::uint32_t skinnedMeshMaterialPipelineId = rc::cfg::MaterialPipelineIds::Mesh3DSkinned,
        std::uint32_t unskinnedMeshMaterialPipelineId =
            rc::cfg::MaterialPipelineIds::Mesh3DMaterial);

protected:
    /**
     * @brief Creates a capsule the covers the model
     */
    virtual std::unique_ptr<JPH::Shape> createPhysicsShape() override;

private:
    using Tx = ecs::Transaction<
        ecs::tx::EntityWrite, ecs::tx::ComponentRead<>,
        ecs::tx::ComponentWrite<com::Transform3D, com::SkinnedMesh, com::MaterialInstance,
                                com::Skeleton, com::SkeletonIndexLink, com::BasicMesh, com::Bone>>;

    struct Child {
        ecs::Entity entity;
        std::variant<com::SkinnedMesh*, com::BasicMesh*> mesh;

        Child() = default;
        Child(ecs::Entity entity, com::SkinnedMesh* mesh)
        : entity(entity)
        , mesh(mesh) {}
        Child(ecs::Entity entity, com::BasicMesh* mesh)
        : entity(entity)
        , mesh(mesh) {}
    };

    struct PhysicsData {
        glm::vec2 heightRange;
        glm::vec2 radiusRange;
    };

    ecs::Registry* ecs;
    com::Skeleton* skeleton;
    std::vector<Child> children;
    bool createdMeshOnSelf;
    PhysicsData physicsShapeSettings;

    void processNode(engine::World& world, Tx& tx, std::uint32_t skinnedMaterialPipelineId,
                     std::uint32_t nonSkinnedMaterialPipelineId, com::Skeleton& skeleton,
                     const as::TypedRef<asi::ModelPayload>& model, const mdl::Node& node,
                     ecs::Entity parentEntity);
    void computePhysicsData(const as::TypedRef<asi::ModelPayload>& model,
                            const glm::mat4& parentTransform, PhysicsData& data);

    virtual void onAdd(rc::Scene* scene, rc::UpdateSpeed updateFreq) override;
    virtual void onRemove() override;
};

} // namespace o3d
} // namespace obj
} // namespace bl

#endif
