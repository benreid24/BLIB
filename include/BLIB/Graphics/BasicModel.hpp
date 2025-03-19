#ifndef BLIB_GRAPHICS_BASICMODEL_HPP
#define BLIB_GRAPHICS_BASICMODEL_HPP

#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Graphics/Components/Transform3D.hpp>
#include <BLIB/Graphics/Drawable.hpp>
#include <BLIB/Models.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Resources/Ref.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief Drawable for static models with no skinning or animation
 *
 * @ingroup Graphics
 */
class BasicModel
: public Drawable<com::BasicMesh>
, public bcom::Transform3D {
public:
    /**
     * @brief Does nothing
     */
    BasicModel();

    /**
     * @brief Creates the basic model from the given file
     *
     * @param world The world to create the model in
     * @param file The file to load the model from
     * @param materialPipelineId The material pipeline to render with
     * @return True if the model was able to loaded and created, false on error
     */
    bool create(engine::World& world, const std::string& file,
                std::uint32_t materialPipelineId = rc::Config::MaterialPipelineIds::Mesh3DMaterial);

    /**
     * @brief Creates the basic model from the given source model
     *
     * @param world The world to create the model in
     * @param model The model to render
     * @param materialPipelineId The material pipeline to render with
     * @return True if the model was able to created, false on error
     */
    bool create(engine::World& world, resource::Ref<mdl::Model> model,
                std::uint32_t materialPipelineId = rc::Config::MaterialPipelineIds::Mesh3DMaterial);

private:
    using Tx = ecs::Transaction<
        ecs::tx::EntityWrite, ecs::tx::ComponentRead<>,
        ecs::tx::ComponentWrite<com::Transform3D, com::BasicMesh, com::MaterialInstance>>;

    void createComponents(engine::World& world, Tx& tx, ecs::Entity entity, const mdl::Mesh& src);
    void createChild(engine::World& world, Tx& tx, const mdl::Mesh& src);

    virtual void onAdd(const rc::rcom::SceneObjectRef& sceneRef);
    virtual void onRemove();
};

} // namespace gfx
} // namespace bl

#endif
