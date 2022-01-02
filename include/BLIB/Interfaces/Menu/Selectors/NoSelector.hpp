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
 *
 */
class NoSelector : public Selector {
public:
    using Ptr = std::shared_ptr<NoSelector>;

    /**
     * @brief Creates a new empty selector
     *
     */
    static Ptr create() { return Ptr(new NoSelector()); }

    /**
     * @brief Does nothing
     *
     */
    virtual void render(sf::RenderTarget&, sf::RenderStates, sf::FloatRect) const override {}

private:
    NoSelector() = default;
};

} // namespace menu
} // namespace bl

#endif
