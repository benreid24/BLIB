#ifndef BLIB_RENDER_OBSERVER_HPP
#define BLIB_RENDER_OBSERVER_HPP

#include <BLIB/Render/RenderTarget.hpp>

namespace bl
{
namespace rc
{
class Renderer;

/**
 * @brief Top level class for observers in the renderer. Corresponds to Engine Actors. Each observer
 *        may have it's own scene stack and overlay, but the Renderer may also have a common
 *        scene and overlay for all observers. Observers may use the same scene amongst
 *        themselves and simply render it from different perspectives
 *
 * @ingroup Renderer
 */
class Observer : public RenderTarget {
public:
    /**
     * @brief Releases resources
     */
    virtual ~Observer() = default;

    /**
     * @brief Assigns the screen region to render to. Only valid for virtual observers
     *
     * @param region The scissor to render to
     */
    void assignRegion(const VkRect2D& region);

private:
    const bool isCommon;
    const bool isVirtual;

    // called by Renderer
    Observer(engine::Engine& engine, Renderer& renderer, rg::AssetFactory& factory, bool isCommon,
             bool isVirtual);
    void assignRegion(const sf::Vector2u& windowSize, const sf::Rect<std::uint32_t>& parentRegion,
                      unsigned int observerCount, unsigned int index, bool topBottomFirst);

    friend class bl::rc::Renderer;
};

} // namespace rc
} // namespace bl

#endif
