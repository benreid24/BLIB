#ifndef BLIB_INTERFACES_MENU_SELECTORS_NOSELECTOR_HPP
#define BLIB_INTERFACES_MENU_SELECTORS_NOSELECTOR_HPP

#include <BLIB/Interfaces/Menu/Selector.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief A selector that does not render anything. Useful for menus using some other mechanism to
 *        display the selected item
 *
 * @ingroup Menu
 */
class NoSelector : public Selector {
public:
    using Ptr = std::shared_ptr<NoSelector>;

    /**
     * @brief Creates a new empty selector
     */
    static Ptr create() { return Ptr(new NoSelector()); }

    /**
     * @brief Does nothing
     */
    virtual void doCreate(engine::Engine&, ecs::Entity) override {}

    /**
     * @brief Does nothing
     */
    virtual void notifySelection(ecs::Entity, sf::FloatRect) override {}

    /**
     * @brief Does nothing
     */
    virtual void doSceneAdd(rc::Scene*) override {}

    /**
     * @brief Does nothing
     */
    virtual void doSceneRemove() override {}

    /**
     * @brief Does nothing
     */
    virtual void draw(rc::scene::CodeScene::RenderContext&) override {}

private:
    NoSelector() = default;
};

} // namespace menu
} // namespace bl

#endif
