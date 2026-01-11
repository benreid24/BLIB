#ifndef BLIB_RENDER_GRAPH_TASKS_FADEEFFECTTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_FADEEFFECTTASK_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Basic post effect task that multiplies the rendered color by a factor in the range [0, 1]
 *        and interpolates that factor. Typically used for a fade in or out to black
 *
 * @ingroup Renderer
 */
class FadeEffectTask : public rg::Task {
public:
    /**
     * @brief Creates a new fade task
     *
     * @param fadeTime The time, in seconds, to fade across
     * @param factorStart The starting multiplier
     * @param factorEnd The final multiplier
     */
    FadeEffectTask(float fadeTime, float factorStart = 1.f, float factorEnd = 0.f);

    /**
     * @brief Frees resources
     */
    virtual ~FadeEffectTask();

    /**
     * @brief Resets the fade to interpolate to the new factor from the current factor
     *
     * @param fadeTime The time, in seconds, to fade across
     * @param factorEnd The new factor to fade to
     */
    void fadeTo(float fadeTime, float factorEnd);

    /**
     * @brief Resets the fade to interpolate between the two factors
     *
     * @param fadeTime The time, in seconds, to fade across
     * @param fadeStart The starting multiplier
     * @param fadeEnd The final multiplier
     */
    void fade(float fadeTime, float fadeStart = 1.f, float fadeEnd = 0.f);

    /**
     * @brief Advances the fade by the given amount of time
     *
     * @param dt The elapsed time in seconds
     */
    virtual void update(float dt) override;

    /**
     * @brief Returns whether or not the fade is complete
     */
    bool complete() const { return factor == fadeEnd; }

    /**
     * @brief Returns the current fade factor
     */
    float currentFactor() const { return factor; }

private:
    Renderer* renderer;
    RenderTarget* target;
    Scene* scene;
    buf::IndexBuffer indexBuffer;
    vk::PipelineInstance pipeline;
    float fadeEnd;
    float fadeSpeed;
    float factor;

    virtual void create(const rg::InitContext& ctx) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
