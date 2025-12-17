#include <BLIB/Graphics/ModelSkeletal.hpp>

#include <BLIB/Resources.hpp>

namespace bl
{
namespace gfx
{
ModelSkeletal::ModelSkeletal()
: ecs(nullptr)
, createdMeshOnSelf(nullptr) {}

bool ModelSkeletal::create(engine::World& world, const std::string& file, std::uint32_t mpid) {
    auto model = resource::ResourceManager<mdl::Model>::load(file);
    if (!model) { return false; }
    return create(world, model, mpid);
}

bool ModelSkeletal::create(engine::World& world, resource::Ref<mdl::Model> model,
                           std::uint32_t mpid) {
    ecs = &world.engine().ecs();

    Drawable::createWithMaterial(world, mpid);
    Transform3D::create(world.engine().ecs(), entity());
    Outline3D::init(world.engine().ecs(), entity(), &component());

    Tx tx(world.engine().ecs());

    auto* skeleton = world.engine().ecs().emplaceComponentWithTx<com::Skeleton>(entity(), tx);
    skeleton->init(*model);

    processNode(world, tx, mpid, model, model->getRoot());

    if (!createdMeshOnSelf) {
        destroy();
        return false;
    }

    return true;
}

com::SkinnedMesh* ModelSkeletal::createComponents(engine::World& world, Tx& tx, ecs::Entity entity,
                                                  std::uint32_t mpid,
                                                  const resource::Ref<mdl::Model>& model,
                                                  const mdl::Mesh& src) {
    // meshes have identity transform. bones contain the actual transforms
    world.engine().ecs().emplaceComponentWithTx<com::Transform3D>(entity, tx);

    world.engine().ecs().emplaceComponentWithTx<com::SkeletonIndexLink>(entity, tx);

    auto* mesh = world.engine().ecs().emplaceComponentWithTx<com::SkinnedMesh>(entity, tx);
    mesh->create(world.engine().renderer().vulkanState(), src);

    auto mat = world.engine().renderer().materialPool().getOrCreateFromModelMaterial(
        model->getMaterials().getMaterial(src.getMaterialIndex()));
    auto* matInstance = world.engine().ecs().emplaceComponentWithTx<com::MaterialInstance>(
        entity, tx, world.engine().renderer(), *mesh, mpid, mat);
    mesh->init(matInstance);

    return mesh;
}

void ModelSkeletal::processNode(engine::World& world, Tx& tx, std::uint32_t materialPipelineId,
                                const resource::Ref<mdl::Model>& model, const mdl::Node& node) {
    unsigned int startMesh = 0;

    if (!createdMeshOnSelf && !node.getMeshes().empty()) {
        createdMeshOnSelf             = true;
        startMesh                     = 1;
        const std::uint32_t meshIndex = node.getMeshes().front();
        createComponents(
            world, tx, entity(), materialPipelineId, model, model->getMeshes().getMesh(meshIndex));
    }

    for (unsigned int i = startMesh; i < node.getMeshes().size(); ++i) {
        const std::uint32_t meshIndex = node.getMeshes()[i];
        createChild(world, tx, materialPipelineId, model, model->getMeshes().getMesh(meshIndex));
    }

    // all meshes of all child nodes are direct children of the root entity
    for (const auto& ci : node.getChildren()) {
        const mdl::Node& childNode = model->getNodes().getNode(ci);
        processNode(world, tx, materialPipelineId, model, childNode);
    }
}

void ModelSkeletal::createChild(engine::World& world, Tx& tx, std::uint32_t mpid,
                                const resource::Ref<mdl::Model>& model, const mdl::Mesh& src) {
    auto child = world.createEntity();
    world.engine().ecs().setEntityParent(child, entity());
    auto* mesh = createComponents(world, tx, child, mpid, model, src);
    children.emplace_back(Child{child, mesh});
}

void ModelSkeletal::onAdd(rc::Scene* scene, rc::UpdateSpeed updateFreq) {
    // TODO - when to update bone indices on vertices?
    for (auto& child : children) { child.mesh->addToScene(*ecs, child.entity, scene, updateFreq); }
}

void ModelSkeletal::onRemove() {
    for (auto& child : children) { child.mesh->removeFromScene(*ecs, child.entity); }
}

} // namespace gfx
} // namespace bl
