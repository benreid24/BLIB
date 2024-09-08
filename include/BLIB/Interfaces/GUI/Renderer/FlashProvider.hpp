#ifndef BLIB_GUI_RENDERER_FLASHPROVIDER_HPP
#define BLIB_GUI_RENDERER_FLASHPROVIDER_HPP

#include <BLIB/Interfaces/GUI/Renderer/Component.hpp>
#include <functional>

namespace bl
{
namespace gui
{
class Element;

namespace rdr
{
/**
 * @brief Base class for providers of element flashing. Elements flash when they are forcing
 *        themselves to remain focused even when other elements are clicked
 *
 * @ingroup GUI
 */
class FlashProvider {
public:
    using Ptr     = std::unique_ptr<FlashProvider>;
    using Factory = std::function<Ptr()>;

    /**
     * @brief Destroys the provider
     */
    virtual ~FlashProvider() = default;

    /**
     * @brief Updates the provider
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Called when an element needs to flash
     *
     * @param element The element to flash
     */
    virtual void flashElement(Element* element) = 0;

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
