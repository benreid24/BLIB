#include <BLIB/Graphics/BasicModel.hpp>

#include <BLIB/Resources.hpp>

namespace bl
{
namespace gfx
{
BasicModel::BasicModel() {}

bool BasicModel::create(engine::World& world, const std::string& file, std::uint32_t mpid) {
    auto model = resource::ResourceManager<mdl::Model>::load(file);
    if (!model) { return false; }
    return create(world, model, mpid);
}

bool BasicModel::create(engine::World& world, resource::Ref<mdl::Model> model, std::uint32_t mpid) {
    ecs = &world.engine().ecs();
    model->mergeChildren();
    if (model->getRoot().getMeshes().empty()) { return false; }

    Drawable::createWithMaterial(world, mpid);
    Transform3D::create(world.engine().ecs(), entity());
    Outline3D::init(world.engine().ecs(), entity(), &component());

    Tx tx(world.engine().ecs());

    // put the first mesh directly on this entity
    createComponents(world, tx, entity(), mpid, model, model->getRoot().getMeshes().front());

    // put the rest of the meshes on children
    children.reserve(model->getRoot().getMeshes().size() - 1);
    for (std::size_t i = 1; i < model->getRoot().getMeshes().size(); ++i) {
        createChild(world, tx, mpid, model, model->getRoot().getMeshes()[i]);
    }

    return true;
}

com::BasicMesh* BasicModel::createComponents(engine::World& world, Tx& tx, ecs::Entity entity,
                                             std::uint32_t mpid,
                                             const resource::Ref<mdl::Model>& model,
                                             const mdl::Mesh& src) {
    world.engine().ecs().emplaceComponentWithTx<com::Transform3D>(entity, tx);
    auto* mesh = world.engine().ecs().emplaceComponentWithTx<com::BasicMesh>(entity, tx);
    mesh->create(world.engine().renderer().vulkanState(), src);
    auto mat = world.engine().renderer().materialPool().getOrCreateFromModelMaterial(
        model->getMaterials().getMaterial(src.getMaterialIndex()));
    auto* matInstance = world.engine().ecs().emplaceComponentWithTx<com::MaterialInstance>(
        entity, tx, world.engine().renderer(), *mesh, mpid, mat);
    mesh->init(matInstance);
    return mesh;
}

void BasicModel::createChild(engine::World& world, Tx& tx, std::uint32_t mpid,
                             const resource::Ref<mdl::Model>& model, const mdl::Mesh& src) {
    auto child = world.createEntity();
    world.engine().ecs().setEntityParent(child, entity());
    auto* mesh = createComponents(world, tx, child, mpid, model, src);
    children.emplace_back(Child{child, mesh});
}

void BasicModel::onAdd(rc::Scene* scene, rc::UpdateSpeed updateFreq) {
    for (auto& child : children) { child.mesh->addToScene(*ecs, child.entity, scene, updateFreq); }
}

void BasicModel::onRemove() {
    for (auto& child : children) { child.mesh->removeFromScene(*ecs, child.entity); }
}

} // namespace gfx
} // namespace bl
