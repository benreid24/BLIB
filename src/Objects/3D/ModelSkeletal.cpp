#include "ModelSkeletal.hpp"
#include <BLIB/Objects/3D/ModelSkeletal.hpp>

#include <BLIB/Components/Bone.hpp>

namespace bl
{
namespace obj
{
namespace o3d
{
ModelSkeletal::ModelSkeletal()
: ecs(nullptr)
, skeleton(nullptr)
, createdMeshOnSelf(false) {}

bool ModelSkeletal::create(engine::World& world, const std::string& file,
                           std::uint32_t skinnedMaterial, std::uint32_t unskinnedMaterial) {
    auto model = world.engine().assets().getAssetFromSourcePath<asi::ModelPayload>(file);
    if (!model) { return false; }
    return create(world, model, skinnedMaterial, unskinnedMaterial);
}

bool ModelSkeletal::create(engine::World& world, as::TypedRef<asi::ModelPayload> model,
                           std::uint32_t skinnedMaterial, std::uint32_t unskinnedMaterial) {
    if (model->getNodes().size() == 0) { return false; }

    ecs = &world.engine().ecs();

    DrawableObject3D::createWithMaterial(world, skinnedMaterial);
    DrawableObject::setRequiresComponentOnSelf(false);

    Tx tx(world.engine().ecs());

    skeleton = world.engine().ecs().emplaceComponentWithTx<com::Skeleton>(entity(), tx);
    skeleton->bones.resize(model->getBones().numBones(), {});
    skeleton->worldTransform = &getTransform();

    skeleton->animations.reserve(model->getAnimationCount());
    for (unsigned int i = 0; i < model->getAnimationCount(); ++i) {
        skeleton->animations.emplace_back(model->getAnimationRef(i));
    }

    processNode(world,
                tx,
                skinnedMaterial,
                unskinnedMaterial,
                *skeleton,
                model,
                model->getRoot(),
                entity());
    tx.unlock();

    // compute and store physics bounds
    PhysicsData physicsData;
    physicsData.heightRange = {std::numeric_limits<float>::max(),
                               std::numeric_limits<float>::lowest()};
    physicsData.radiusRange = {std::numeric_limits<float>::max(),
                               std::numeric_limits<float>::lowest()};
    computePhysicsData(model, model->getRoot(), glm::identity<glm::mat4>(), physicsData);
    physicsShapeSettings =
        JPH::CapsuleShapeSettings((physicsData.heightRange.y - physicsData.heightRange.x) * 0.5f,
                                  (physicsData.radiusRange.y - physicsData.radiusRange.x) * 0.5f);

    // validate that all bones populated
    for (auto& bone : skeleton->bones) {
        if (!bone.bone) {
            BL_LOG_ERROR << "Skeletal model did not populate all declared bones!";
            destroy();
            return false;
        }
    }

    if (!createdMeshOnSelf && children.empty()) {
        destroy();
        return false;
    }

    return true;
}

void ModelSkeletal::processNode(engine::World& world, Tx& tx,
                                std::uint32_t skinnedMaterialPipelineId,
                                std::uint32_t nonSkinnedMaterialPipelineId, com::Skeleton& skeleton,
                                const as::TypedRef<asi::ModelPayload>& model, const mdl::Node& node,
                                ecs::Entity parentEntity) {
    ecs::Entity nodeEntity = world.createEntity(tx);
    world.engine().ecs().setEntityParent(nodeEntity, parentEntity, tx);

    // Create transform on node entity and initialize to bind pose
    com::Transform3D* transform =
        world.engine().ecs().emplaceComponentWithTx<com::Transform3D>(nodeEntity, tx);
    transform->setTransform(node.getTransform());

    // create the bone component and setup links if this node is a bone
    if (node.getBoneIndex().has_value()) {
        com::Bone* bone = world.engine().ecs().emplaceComponentWithTx<com::Bone>(nodeEntity, tx);
        bone->nodeBindPoseLocal = node.getTransform();
        bone->transform         = transform;
        bone->boneIndex         = node.getBoneIndex().value();
        bone->boneOffset        = model->getBones().getBone(bone->boneIndex).transform;
        skeleton.bones[bone->boneIndex].transform = transform;
        skeleton.bones[bone->boneIndex].bone      = bone;
    }

    // create entities per mesh
    for (const std::uint32_t meshIndex : node.getMeshes()) {
        const mdl::Mesh& mesh  = model->getMeshes().getMesh(meshIndex);
        ecs::Entity meshEntity = ecs::InvalidEntity;

        // try to put one mesh on root entity
        if (!createdMeshOnSelf && mesh.getIsSkinned()) {
            meshEntity        = entity();
            createdMeshOnSelf = true;
        }
        else {
            meshEntity = world.createEntity(tx);

            // mesh transforms are always identity
            world.engine().ecs().emplaceComponentWithTx<com::Transform3D>(meshEntity, tx);
        }

        // mesh material
        const std::uint32_t materialId =
            mesh.getIsSkinned() ? skinnedMaterialPipelineId : nonSkinnedMaterialPipelineId;
        auto mat = world.engine().renderer().materialPool().getOrCreateFromAsset(
            model->getMaterialRef(mesh.getMaterialIndex()));

        // skinned meshes are direct children of the root node. bone transforms provide the
        // accumulated node transforms
        if (mesh.getIsSkinned()) {
            if (meshEntity != entity()) {
                world.engine().ecs().setEntityParent(meshEntity, entity(), tx);

                auto* meshComponent =
                    world.engine().ecs().emplaceComponentWithTx<com::SkinnedMesh>(meshEntity, tx);
                meshComponent->create(world.engine().renderer(), mesh);

                children.emplace_back(meshEntity, meshComponent);
            }
            else { // add mesh to root
                component().create(world.engine().renderer(), mesh);
                auto* matInstance =
                    world.engine().ecs().emplaceComponentWithTx<com::MaterialInstance>(
                        meshEntity, tx, world.engine().renderer(), component(), materialId, mat);
                component().init(matInstance);
            }
            world.engine().ecs().emplaceComponentWithTx<com::SkeletonIndexLink>(meshEntity, tx);
        }
        else {
            world.engine().ecs().setEntityParent(meshEntity, nodeEntity, tx);

            auto* meshComponent =
                world.engine().ecs().emplaceComponentWithTx<com::BasicMesh>(meshEntity, tx);
            meshComponent->create(world.engine().renderer(), mesh);

            children.emplace_back(meshEntity, meshComponent);
        }

        // create material instance on the mesh (root gets it from Drawable::createWithMaterial
        if (meshEntity != entity()) {
            std::visit(
                [&world, &tx, &mat, meshEntity, materialId](auto& meshComponent) {
                    auto* matInstance =
                        world.engine().ecs().emplaceComponentWithTx<com::MaterialInstance>(
                            meshEntity,
                            tx,
                            world.engine().renderer(),
                            *meshComponent,
                            materialId,
                            mat);
                    meshComponent->init(matInstance);
                },
                children.back().mesh);
        }
    }

    // create entities recursively for child nodes
    for (std::uint32_t ci : node.getChildren()) {
        const mdl::Node& childNode = model->getNodes().getNode(ci);
        processNode(world,
                    tx,
                    skinnedMaterialPipelineId,
                    nonSkinnedMaterialPipelineId,
                    skeleton,
                    model,
                    childNode,
                    nodeEntity);
    }
}

void ModelSkeletal::onAdd(rc::Scene* scene, rc::UpdateSpeed updateFreq) {
    for (auto& child : children) {
        std::visit([this, &child, scene, updateFreq](
                       auto& mesh) { mesh->addToScene(*ecs, child.entity, scene, updateFreq); },
                   child.mesh);
    }
}

void ModelSkeletal::onRemove() {
    for (auto& child : children) {
        std::visit([this, &child](auto& mesh) { mesh->removeFromScene(*ecs, child.entity); },
                   child.mesh);
    }
}

std::unique_ptr<JPH::Shape> ModelSkeletal::createPhysicsShape() {
    JPH::Shape::ShapeResult result;
    return std::make_unique<JPH::CapsuleShape>(physicsShapeSettings, result);
}

void ModelSkeletal::computePhysicsData(const as::TypedRef<asi::ModelPayload>& model,
                                       const mdl::Node& node, const glm::mat4& parentTransform,
                                       PhysicsData& data) {
    const glm::mat4 globalTransform = parentTransform * node.getTransform();

    for (const std::uint32_t meshIndex : node.getMeshes()) {
        const auto& mesh = model->getMeshes().getMesh(meshIndex);

        for (const auto& vertex : mesh.getVertices()) {
            const glm::vec4 transformedVertex = globalTransform * glm::vec4(vertex.pos, 1.0f);
            const float r      = glm::length(glm::vec2(transformedVertex.x, transformedVertex.z));
            data.heightRange.x = std::min(data.heightRange.x, transformedVertex.y);
            data.heightRange.y = std::max(data.heightRange.y, transformedVertex.y);
            data.radiusRange.x = std::min(data.radiusRange.x, r);
            data.radiusRange.y = std::max(data.radiusRange.y, r);
        }
    }

    for (std::uint32_t ci : node.getChildren()) {
        const mdl::Node& childNode = model->getNodes().getNode(ci);
        computePhysicsData(model, childNode, globalTransform, data);
    }
}

} // namespace o3d
} // namespace obj
} // namespace bl
