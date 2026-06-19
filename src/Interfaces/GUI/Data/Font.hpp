#ifndef BLIB_GUI_DATA_FONT_HPP
#define BLIB_GUI_DATA_FONT_HPP

#include <BLIB/Assets/Builtin/FontPayload.hpp>
#include <BLIB/Assets/TypedRef.hpp>

namespace bl
{
namespace as
{
class Repository;
}
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
     * @param repo The repository to fetch the font from
     */
    static as::TypedRef<asi::FontPayload> get(as::Repository& repo);
};

} // namespace gui
} // namespace bl

#endif
