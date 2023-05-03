#ifndef BLIB_RENDER_DESCRIPTORS_SCENEDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENEDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Transfers/GenericBuffer.hpp>
#include <BLIB/Render/Util/PerFrame.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace render
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
     * @brief Creates the descriptor set instance
     */
    SceneDescriptorSetInstance();

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~SceneDescriptorSetInstance() = default;

    /**
     * @brief Called by Scene when a new observer is added to it
     *
     * @return Index of the new observer
     */
    std::uint32_t registerObserver();

    /**
     * @brief Updates the camera value for the given observer
     *
     * @param observerIndex Index of the observer to update
     * @param projView Camera matrix for the given observer
     */
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);

protected:
    std::array<glm::mat4, Config::MaxSceneObservers> observerCameras;
    // TODO - other common scene data

    /**
     * @brief Returns the number of observers currently in the scene
     */
    constexpr std::uint32_t observerCount() const;

private:
    std::uint32_t nextObserverIndex;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr std::uint32_t SceneDescriptorSetInstance::observerCount() const {
    return nextObserverIndex;
}

} // namespace ds
} // namespace render
} // namespace bl

#endif
