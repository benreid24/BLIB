#ifndef BLIB_MENU_SELECTOR_HPP
#define BLIB_MENU_SELECTOR_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <memory>

namespace bl
{
namespace engine
{
class Engine;
}

namespace menu
{
/**
 * @brief Base class for selector items that indicate which Menu item is active
 *
 * @ingroup Menu
 *
 */
class Selector {
public:
    typedef std::shared_ptr<Selector> Ptr;

    /**
     * @brief Destroys the selector
     */
    virtual ~Selector() = default;

    /**
     * @brief Called when the selector is added to a menu
     *
     * @param engine The game engine instance
     * @param parent The parent entity id
     */
    virtual void doCreate(engine::Engine& engine, ecs::Entity parent) = 0;

    /**
     * @brief Called when the selector should select an item
     *
     * @param item The ECS id of the selected item
     * @param itemArea The menu local area of the selected item
     */
    virtual void notifySelection(ecs::Entity item, sf::FloatRect itemArea) = 0;

    /**
     * @brief Called when the selector should be added to the overlay
     *
     * @param overlay The overlay to add to
     */
    virtual void doSceneAdd(rc::Overlay* overlay) = 0;

    /**
     * @brief Called when the selector should be removed from the overlay
     */
    virtual void doSceneRemove() = 0;
};

} // namespace menu
} // namespace bl

#endif
