#ifndef BLIB_GUI_DATA_FONT_HPP
#define BLIB_GUI_DATA_FONT_HPP

#include <BLIB/Resources/Loader.hpp>
#include <SFML/Graphics/Font.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Data struct that provides access to the built in font. The font may be left out of
 *        the build by defining BLIB_NO_FONT
 *
 * @ingroup GUI
 *
 */
struct Font {
    /**
     * @brief Returns a loaded sf::Font resource from the built in font
     *
     */
    static resource::Resource<sf::Font>::Ref get();
};

} // namespace gui
} // namespace bl

#endif
