#include <BLIB/Graphics/ModelStatic.hpp>

#include <BLIB/Resources.hpp>

namespace bl
{
namespace gfx
{
namespace
{
constexpr glm::mat4 Identity = glm::mat4(1.f);
}

ModelStatic::ModelStatic() {}

bool ModelStatic::create(engine::World& world, const std::string& file, std::uint32_t mpid) {
    auto model = resource::ResourceManager<mdl::Model>::load(file);
    if (!model) { return false; }
    return create(world, model, mpid);
}

bool ModelStatic::create(engine::World& world, resource::Ref<mdl::Model> model,
                         std::uint32_t mpid) {
    ecs = &world.engine().ecs();

    Drawable::createWithMaterial(world, mpid);
    Transform3D::create(world.engine().ecs(), entity());
    Outline3D::init(world.engine().ecs(), entity(), &component());

    drawableEntities.reserve(model->getMeshes().getMeshCount());

    Tx tx(world.engine().ecs());
    processNode(world, tx, entity(), mpid, model, model->getRoot());

    // TODO - HACK - this is a workaround for lack of first-class support for multiple draws/meshes
    // for a single entity in the renderer. Should rethink the renderer object <-> ECS entity model
    if (component().getDrawParameters().indexCount == 0) {
        component().create(world.engine().renderer().vulkanState(), 1, 3);
        component().gpuBuffer.indices()  = {0, 0, 0};
        component().gpuBuffer.vertices() = {rc::prim::Vertex3D(glm::vec3(0.f, 0.f, 0.f))};
        component().gpuBuffer.queueTransfer();
    }

    return true;
}

com::BasicMesh* ModelStatic::createComponents(engine::World& world, Tx& tx, ecs::Entity entity,
                                              std::uint32_t mpid,
                                              const resource::Ref<mdl::Model>& model,
                                              const mdl::Mesh& src, const glm::mat4& tfrm) {
    com::Transform3D* transform =
        world.engine().ecs().emplaceComponentWithTx<com::Transform3D>(entity, tx);
    transform->setTransform(tfrm);

    auto* mesh = world.engine().ecs().emplaceComponentWithTx<com::BasicMesh>(entity, tx);
    mesh->create(world.engine().renderer().vulkanState(), src);

    auto mat = world.engine().renderer().materialPool().getOrCreateFromModelMaterial(
        model->getMaterials().getMaterial(src.getMaterialIndex()));
    auto* matInstance = world.engine().ecs().emplaceComponentWithTx<com::MaterialInstance>(
        entity, tx, world.engine().renderer(), *mesh, mpid, mat);
    mesh->init(matInstance);

    return mesh;
}

void ModelStatic::createChild(engine::World& world, Tx& tx, ecs::Entity parent, std::uint32_t mpid,
                              const resource::Ref<mdl::Model>& model, const mdl::Mesh& src) {
    auto child = world.createEntity(tx);
    world.engine().ecs().setEntityParent(child, parent, tx);
    auto* mesh = createComponents(world, tx, child, mpid, model, src, Identity);
    drawableEntities.emplace_back(EntityNode{child, mesh});
}

void ModelStatic::processNode(engine::World& world, Tx& tx, ecs::Entity parent, std::uint32_t mpid,
                              const resource::Ref<mdl::Model>& model, const mdl::Node& node) {
    const bool isRoot               = (&node == &model->getRoot());
    const ecs::Entity currentEntity = isRoot ? entity() : world.createEntity(tx);

    if (currentEntity != parent) {
        world.engine().ecs().setEntityParent(currentEntity, parent, tx);
    }

    bool meshOnSelf = false;
    for (const auto& meshIdx : node.getMeshes()) {
        if (meshOnSelf) {
            createChild(world, tx, currentEntity, mpid, model, model->getMeshes().getMesh(meshIdx));
        }
        else {
            meshOnSelf = true;
            auto* mesh = createComponents(world,
                                          tx,
                                          currentEntity,
                                          mpid,
                                          model,
                                          model->getMeshes().getMesh(node.getMeshes().front()),
                                          node.getTransform());
            drawableEntities.emplace_back(EntityNode{currentEntity, mesh});
        }
    }

    for (const auto& childNode : node.getChildren()) {
        processNode(world, tx, currentEntity, mpid, model, model->getNodes().getNode(childNode));
    }
}

void ModelStatic::onAdd(rc::Scene* scene, rc::UpdateSpeed updateFreq) {
    for (auto& child : drawableEntities) {
        child.mesh->addToScene(*ecs, child.entity, scene, updateFreq);
    }
}

void ModelStatic::onRemove() {
    for (auto& child : drawableEntities) { child.mesh->removeFromScene(*ecs, child.entity); }
}

} // namespace gfx
} // namespace bl
