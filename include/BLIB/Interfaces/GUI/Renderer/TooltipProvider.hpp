#ifndef BLIB_GUI_RENDERER_TOOLTIPPROVIDER_HPP
#define BLIB_GUI_RENDERER_TOOLTIPPROVIDER_HPP

#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <functional>
#include <glm/glm.hpp>

namespace bl
{
namespace gui
{
class Element;

namespace rdr
{
/**
 * @brief Base class for providers of element tooltips
 *
 * @ingroup GUI
 */
class TooltipProvider {
public:
    using Ptr     = std::unique_ptr<TooltipProvider>;
    using Factory = std::function<Ptr()>;

    /**
     * @brief Destroys the provider
     */
    virtual ~TooltipProvider() = default;

    /**
     * @brief Updates the provider
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Called when an element needs a tooltip displayed
     *
     * @param element The element to show the tooltip for
     */
    virtual void displayTooltip(Element* element, const glm::vec2& mousePos) = 0;

    /**
     * @brief Called when the current tooltip, if any, should be dismissed
     */
    virtual void dismissTooltip() = 0;

    /**
     * @brief Called once before use. Derived classes should create resources here
     *
     * @param world The world to create entities in
     */
    virtual void doCreate(engine::World& world) = 0;

    /**
     * @brief Called when the provider should be added to a scene
     *
     * @param overlay The scene to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) = 0;

    /**
     * @brief Called when the provider should be removed from the scene
     */
    virtual void doSceneRemove() = 0;

    /**
     * @brief Called when an element is destroyed or removed
     *
     * @param element The element being destroyed or removed
     */
    virtual void notifyDestroyed(const Element* element) = 0;
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
