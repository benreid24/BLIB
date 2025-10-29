#ifndef BLIB_RENDER_RESOURCES_GLOBALDESCRIPTORS_HPP
#define BLIB_RENDER_RESOURCES_GLOBALDESCRIPTORS_HPP

#include <BLIB/Render/Buffers/BufferDoubleStaged.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Buffers/StaticUniformBuffer.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Settings.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Vulkan.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}
namespace rc
{
class Renderer;

namespace res
{
class TexturePool;
class MaterialPool;

/**
 * @brief Top level manager for global descriptor data such the texture and material pools
 *
 * @ingroup Renderer
 */
class GlobalDescriptors {
public:
    struct DynamicSettingsUniform {
        float currentHdrExposure;
    };

    /**
     * @brief Creates the renderer global descriptors
     *
     * @param engine The game engine instance
     * @param renderer The renderer instance
     * @param texturePool The texture pool
     * @param materialPool The material pool
     */
    GlobalDescriptors(engine::Engine& engine, Renderer& renderer, TexturePool& texturePool,
                      MaterialPool& materialPool);

    /**
     * @brief Returns the layout of the descriptor set containing the textures
     */
    VkDescriptorSetLayout getDescriptorLayout() const;

    /**
     * @brief Helper method to bind the descriptor set
     *
     * @param commandBuffer Command buffer to issue bind command into
     * @param pipelineBindPoint The bind point of the active pipeline
     * @param pipelineLayout The layout of the active pipeline
     * @param setIndex The index to bind to
     * @param forRenderTexture True to omit RT images, false to bind all
     */
    void bindDescriptors(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint,
                         VkPipelineLayout pipelineLayout, std::uint32_t setIndex,
                         bool forRenderTexture);

    /**
     * @brief Creates the global descriptor sets and initializes the texture and material pools
     */
    void init();

    /**
     * @brief Cleans up renderer resources
     */
    void cleanup();

    /**
     * @brief Updates descriptor sets that have changed
     */
    void onFrameStart();

    /**
     * @brief Called when an engine frame tick occurs
     *
     * @param dt The simulated dt
     * @param realDt The real elapsed dt
     * @param residual The simulated unaccounted for time
     * @param realResidual The real unaccounted for time
     */
    void notifyUpdateTick(float dt, float realDt, float residual, float realResidual);

    /**
     * @brief Updates the render settings payload on the GPU for the shaders
     *
     * @param settings The new renderer settings
     */
    void updateSettings(const Settings& settings);

    /**
     * @brief Returns the ssbo for the dynamic settings
     */
    buf::StaticSSBO<DynamicSettingsUniform>& getDynamicSettingsBuffer() {
        return dynamicSettingsBuffer;
    }

private:
    struct SettingsUniform {
        Settings::AutoHdrSettings hdrSettings;
        float gamma;
    };

    struct FrameDataUniform {
        float dt;
        float realDt;
        float residual;
        float realResidual;
        float frameDt;
        float realFrameDt;
    };

    engine::Engine& engine;
    Renderer& renderer;
    TexturePool& texturePool;
    MaterialPool& materialPool;
    buf::StaticUniformBuffer<SettingsUniform> settingsBuffer;
    buf::BufferDoubleStagedUBO<FrameDataUniform> frameDataBuffer;
    buf::StaticSSBO<DynamicSettingsUniform> dynamicSettingsBuffer;
    FrameDataUniform accumulatedTimings;
    sf::Clock frameTimer;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    vk::PerFrame<VkDescriptorSet> descriptorSets;
    vk::PerFrame<VkDescriptorSet> rtDescriptorSets;
    ds::SetWriteHelper descriptorWriter;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkDescriptorSetLayout GlobalDescriptors::getDescriptorLayout() const {
    return descriptorSetLayout;
}

} // namespace res
} // namespace rc
} // namespace bl

#endif
