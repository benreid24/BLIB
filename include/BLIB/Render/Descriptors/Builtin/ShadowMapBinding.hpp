#ifndef BLIB_RENDER_DESCRIPTORS_SHADOWMAPBINDING_HPP
#define BLIB_RENDER_DESCRIPTORS_SHADOWMAPBINDING_HPP

#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/Generic/Binding.hpp>
#include <BLIB/Render/Lighting/Scene3DLighting.hpp>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Binding for light camera matrices for rendering shadow maps
 *
 * @ingroup Renderer
 */
class ShadowMapBinding : public Binding {
public:
    /// No individual payload for this binding
    using TPayload = void;

    /**
     * @brief Creates the binding
     */
    ShadowMapBinding()
    : Binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
    , lighting(nullptr) {}

    /**
     * @brief Sets the lighting info to populate matrices from
     *
     * @param l The scene lighting
     */
    void setLighting(lgt::Scene3DLighting& l) { lighting = &l; }

private:
    struct Payload {
        glm::mat4 projection;
        glm::mat4 view;
    };

    buf::UniformBuffer<Payload> cameraMatrices;
    lgt::Scene3DLighting* lighting;

    DescriptorSetInstance::EntityBindMode getBindMode() const override {
        return DescriptorSetInstance::EntityBindMode::Bindless;
    }
    DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const override {
        return DescriptorSetInstance::SpeedBucketSetting::SpeedAgnostic;
    }

    void init(vk::VulkanState& vulkanState,
              DescriptorComponentStorageCache& storageCache) override {
        cameraMatrices.create(vulkanState, Config::MaxSpotShadows + Config::MaxPointShadows * 6);
        cameraMatrices.transferEveryFrame();
    }

    void writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed speed,
                  std::uint32_t frameIndex) override {
        VkDescriptorBufferInfo& bufferInfo = writer.getNewBufferInfo();
        bufferInfo.buffer = cameraMatrices.gpuBufferHandles().getRaw(frameIndex).getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range  = cameraMatrices.alignedUniformSize() * 6;

        VkWriteDescriptorSet& write = writer.getNewSetWrite(set);
        write.dstBinding            = getBindingIndex();
        write.pBufferInfo           = &bufferInfo;
        write.descriptorType        = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    }

    bool allocateObject(ecs::Entity, scene::Key) override { return true; }

    void releaseObject(ecs::Entity, scene::Key) override {}

    void onFrameStart() override {
        if (!lighting) { return; }

        // sunlight camera
        // TODO - set matrices dynamically from camera (or cascaded shadow maps)
        const lgt::SunLight3D& sun = lighting->getSun();
        Payload& sunlightCam       = cameraMatrices[0];
        sunlightCam.projection     = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 100.f);
        sunlightCam.view =
            glm::lookAt(lighting->getSunPosition(), glm::vec3(0.f), Config::UpDirection);

        // spot lights
        for (std::uint32_t i = 0; i < lighting->getSpotShadowCount(); ++i) {
            lgt::SpotLight3D& spot = lighting->getSpotlightUnsafe(i);
            // TODO - calculate far plane from attenuation
            cameraMatrices[i + 1].projection =
                glm::perspective(spot.outerCutoff * 2.f, 1.f, 0.1f, 100.f);
            cameraMatrices[i + 1].view =
                glm::lookAt(spot.pos, spot.pos + spot.dir, Config::UpDirection);
        }

        // point lights
        for (unsigned int i = 0; i < lighting->getPointLightCount(); ++i) {
            lgt::PointLight3D& point = lighting->getPointLightUnsafe(i);
            // TODO - calculate far plane from attenuation
            const glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.f);
            for (unsigned int j = 0; j < 6; ++j) {
                Payload& pointCam   = cameraMatrices[(i + 1) * 6 + j];
                pointCam.projection = proj;
                pointCam.view       = glm::lookAt(
                    point.pos, point.pos + Config::CubemapDirections[j], Config::UpDirection);
            }
        }
    }

    void* getPayload() override { return nullptr; }

    bool staticDescriptorUpdateRequired() const override { return false; }

    bool dynamicDescriptorUpdateRequired() const override { return false; }
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
