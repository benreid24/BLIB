#ifndef BLIB_RENDER_SCENES_CODESCENE_HPP
#define BLIB_RENDER_SCENES_CODESCENE_HPP

#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
#include <BLIB/Render/Lighting/Scene2DLighting.hpp>
#include <BLIB/Render/Scenes/CodeSceneObject.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObjectStorage.hpp>
#include <BLIB/Signals/Emitter.hpp>
#include <functional>

namespace bl
{
namespace rc
{
namespace scene
{
/**
 * @brief A special type of scene that performs rendering by deferring to a user-provided callback
 *        that then issues draw commands manually. Useful for complex rendering logic that is hard
 *        to represent with the provided scene types
 *
 * @ingroup Renderer
 */
class CodeScene : public Scene {
public:
    /**
     * @brief Context that gets passed to the render callback
     */
    class RenderContext {
    public:
        /**
         * @brief Issues the commands to render the given object. All BLIB renderable classes
         *        provide their own methods to render which should be preferred to this one
         *
         * @param object The object to render
         */
        void renderObject(rcom::DrawableBase& object);

        /**
         * @brief Issues the command to set the viewport used for rendering
         *
         * @param viewport The viewport to set
         * @param setScissor Whether to set the scissor to the same dimensions or not
         */
        void setViewport(const VkViewport& viewport, bool setScissor = true);

        /**
         * @brief Sets the scissor to use for rendering
         *
         * @param scissor The scissor to use
         * @param setViewport Whether to set the viewport to the same dimensions or not
         */
        void setScissor(const VkRect2D& scissor, bool setViewport = false);

        /**
         * @brief Resets the scissor to its original value
         */
        void resetScissor();

        /**
         * @brief Resets the viewport to its original value
         */
        void resetViewport();

        /**
         * @brief Resets both the viewport and scissor to their original values
         */
        void resetViewportAndScissor();

    private:
        scene::SceneRenderContext& renderContext;

        RenderContext(scene::SceneRenderContext& renderContext);

        friend class CodeScene;
    };

    /// Signature of the callback that is called when rendering occurs
    using RenderCallback = std::function<void(RenderContext&)>;

    /**
     * @brief Creates the code scene
     *
     * @param engine The game engine instance
     * @param renderCallback The method that will render the objects in the scene
     */
    CodeScene(engine::Engine& engine, RenderCallback&& renderCallback);

    /**
     * @brief Destroys the code scene
     */
    virtual ~CodeScene();

    /**
     * @brief Returns the scene lighting manager for this scene
     */
    lgt::Scene2DLighting& getLighting() { return lighting; }

private:
    const RenderCallback renderCallback;
    SceneObjectStorage<CodeSceneObject> objects;
    lgt::Scene2DLighting lighting;
    sig::Emitter<event::SceneObjectRemoved> emitter;

    virtual void renderOpaqueObjects(scene::SceneRenderContext& context) override;
    virtual void renderTransparentObjects(scene::SceneRenderContext& context) override;
    virtual std::unique_ptr<cam::Camera> createDefaultCamera() override;
    virtual void setDefaultNearAndFarPlanes(cam::Camera& camera) const override;
    virtual SceneObject* doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                               UpdateSpeed updateFreq) override;
    virtual void doBatchChange(const BatchChange& change,
                               mat::MaterialPipeline* ogPipeline) override;
    virtual void doObjectRemoval(SceneObject* object, mat::MaterialPipeline* pipeline) override;
    virtual void doRegisterObserver(RenderTarget* target, std::uint32_t observerIndex) override;
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
