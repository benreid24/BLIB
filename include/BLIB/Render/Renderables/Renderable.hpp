#ifndef BLIB_RENDER_RENDERABLES_RENDERABLE_HPP
#define BLIB_RENDER_RENDERABLES_RENDERABLE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/StagePipelines.hpp>
#include <BLIB/Render/Uniforms/PushConstants.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <mutex>

namespace bl
{
namespace render
{
class Scene;
struct SceneObject;
class Scene;

/**
 * @brief Base class for renderable objects. Renderables are owned by calling code and internally
 *        link to Objects which are owned by the Renderer. All Renderables are backed by index
 *        buffers, although the vertex type is allowed to change
 *
 * @ingroup Renderer
 */
class Renderable {
public:
    /**
     * @brief Move constructor. Assumes ownership of copy's object if it has one
     *
     * @param copy The Renderable to move from
     */
    Renderable(Renderable&& copy);

    /**
     * @brief Destroy the Renderable. Removes the owned SceneObject from the renderer if it has one
     *
     */
    virtual ~Renderable();

    /**
     * @brief Adds this Renderable to the given scene. Removes from the prior scene, if any
     *
     * @param scene The scene to add to
     */
    void addToScene(Scene& scene);

    /**
     * @brief Removes this renderable from the given scene
     *
     */
    void removeFromScene();

    /**
     * @brief Show or hide this renderable
     *
     * @param hidden True to hide, false to show
     */
    void setHidden(bool hidden);

    /**
     * @brief Returns whether or not this Renderable is hidden
     *
     */
    bool isHidden() const;

    /**
     * @brief Returns the model transform of this Renderable
     *
     */
    const glm::mat4& getTransform() const;

    /**
     * @brief Sets the texture index in the renderables push constant data
     *
     * @param textureId The id of the texture
     */
    void setTextureId(std::uint32_t textureId);

    /**
     * @brief Sets the material index in the renderables push constant data
     *
     * @param textureId The id of the material
     */
    void setMaterialId(std::uint32_t materialId);

    /**
     * @brief Sets the transform for this renderable
     *
     * @param transform The model transform to apply
     */
    void setTransform(const glm::mat4& transform);

protected:
    /**
     * @brief Construct a new Renderable with sane defaults
     *
     */
    Renderable();

    /**
     * @brief Child classes should implement this to perform custom logic when this renderable is
     *        added to a scene
     *
     */
    virtual void onSceneAdd();

    /**
     * @brief Marks the renderable's push-constant data as needing to be refreshed
     *
     */
    void markPCDirty();

    /**
     * @brief Sets the parameters for the draw call for this object
     *
     * @param params The parameters to render with
     */
    void setDrawParameters(const DrawParameters& params);

    /**
     * @brief Configures rendering during a given render stage with a given pipeline
     *
     * @param stageId The render stage to add to
     * @param pipelineId The pipeline for the given render pass
     */
    void addOrSetStagePipeline(std::uint32_t stageId, std::uint32_t pipelineId);

    /**
     * @brief Removes the renderable from being rendered during the given render pass
     *
     * @param renderStageId The render pass to remove from
     */
    void removeFromStage(std::uint32_t renderStageId);

private:
    Scene* owner;
    std::mutex mutex;
    SceneObject::Handle object;
    PushConstants frameData;
    bool hidden;
    StagePipelines stageMembership;
    DrawParameters drawParams;

    void syncPC();
    void syncDrawParams();

    Renderable(const Renderable&) = delete;
    friend class PipelineBatch;
    friend class Scene;
};

} // namespace render
} // namespace bl

#endif
