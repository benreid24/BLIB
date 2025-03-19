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
    return create(world, model);
}

bool BasicModel::create(engine::World& world, resource::Ref<mdl::Model> model, std::uint32_t mpid) {
    model->mergeChildren();
    if (model->getRoot().getMeshes().empty()) { return false; }

    Drawable::createWithMaterial(world, mpid);
    Transform3D::create(world.engine().ecs(), entity());

    Tx tx(world.engine().ecs());

    // put the first mesh directly on this entity
    createComponents(world, tx, entity(), model->getRoot().getMeshes().front());

    // put the rest of the meshes on children
    for (std::size_t i = 1; i < model->getRoot().getMeshes().size(); ++i) {
        createChild(world, tx, model->getRoot().getMeshes()[i]);
    }
}

void BasicModel::createComponents(engine::World& world, Tx& tx, ecs::Entity entity,
                                  const mdl::Mesh& src) {
    world.engine().ecs().emplaceComponentWithTx<com::Transform3D>(entity, tx);
    auto* mesh = world.engine().ecs().emplaceComponentWithTx<com::BasicMesh>(entity, tx);
    // TODO - init mesh & create material
}

void BasicModel::createChild(engine::World& world, Tx& tx, const mdl::Mesh& src) {
    auto child = world.createEntity();
    world.engine().ecs().setEntityParent(child, entity());
    createComponents(world, tx, child, src);
}

void BasicModel::onAdd(const rc::rcom::SceneObjectRef& sceneRef) {
    // TODO - add all children
}

void BasicModel::onRemove() {
    // TODO - remove all children
}

} // namespace gfx
} // namespace bl
