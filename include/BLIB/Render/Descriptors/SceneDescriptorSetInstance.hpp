#ifndef BLIB_RENDER_DESCRIPTORS_SCENEDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENEDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Base class for descriptor sets for scene data (cameras, lighting, etc)
 *
 * @ingroup Renderer
 */
class SceneDescriptorSetInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Basic uniform struct containing observer info used during render
     */
    struct ObserverInfo {
        glm::mat4 projView;
        glm::vec3 pos;

        ObserverInfo() = default;
        ObserverInfo(const glm::mat4& projView, const glm::vec3& pos)
        : projView(projView)
        , pos(pos) {}
    };

    /**
     * @brief Creates the descriptor set instance
     */
    SceneDescriptorSetInstance();

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~SceneDescriptorSetInstance() = default;

    /**
     * @brief Updates the camera value for the given observer
     *
     * @param observerIndex Index of the observer to update
     * @param info The observer's camera info
     */
    void updateObserverCamera(std::uint32_t observerIndex, const ObserverInfo& info);

protected:
    buf::UniformBuffer<ObserverInfo> cameraBuffer;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
