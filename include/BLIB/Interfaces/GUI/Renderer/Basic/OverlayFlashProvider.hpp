#ifndef BLIB_GUI_RENDERER_BASIC_OVERLAYFLASHPROVIDER_HPP
#define BLIB_GUI_RENDERER_BASIC_OVERLAYFLASHPROVIDER_HPP

#include <BLIB/Graphics/Rectangle.hpp>
#include <BLIB/Interfaces/GUI/Renderer/FlashProvider.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
/**
 * @brief Provides element flashing for attention by rendering a rectangle over the top of the
 *        element and pulsating its transparency
 *
 * @ingroup GUI
 */
class OverlayFlashProvider : public rdr::FlashProvider {
public:
    /**
     * @brief Creates the provider
     */
    OverlayFlashProvider();

    /**
     * @brief Destroys the provider
     */
    virtual ~OverlayFlashProvider() = default;

    /**
     * @brief Updates the flash state
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) override;

    /**
     * @brief Called when an element needs to flash
     *
     * @param element The element to flash
     */
    virtual void flashElement(Element* element) override;

    /**
     * @brief Creates the overlay rectangle
     *
     * @param world The world to create entities in
     */
    virtual void doCreate(engine::World& world) override;

    /**
     * @brief Adds the rectangle to the scene
     *
     * @param overlay The scene to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) override;

    /**
     * @brief Called when the provider should be removed from the scene
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Called when an element is destroyed or removed
     *
     * @param element The element being destroyed or removed
     */
    virtual void notifyDestroyed(const Element* element) override;

private:
    engine::Engine* enginePtr;
    gfx::Rectangle cover;
    Element* currentElement;
    float flashTime;
};

} // namespace defcoms
} // namespace gui
} // namespace bl

#endif
