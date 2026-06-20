#ifndef BLIB_GUI_FONT_HPP
#define BLIB_GUI_FONT_HPP

#include <BLIB/Assets/Builtin/FontPayload.hpp>
#include <BLIB/Assets/TypedRef.hpp>
#include <span>

namespace bl
{
namespace as
{
class Repository;
}
namespace gui
{
/**
 * @brief Data struct that provides access to the built-in font
 *
 * @ingroup GUI
 */
struct Font {
    /**
     * @brief Returns a loaded FontPayload from the built-in font
     *
     * @param repo The repository to fetch the font from
     */
    static as::TypedRef<asi::FontPayload> get(as::Repository& repo);

    /**
     * @brief Returns the raw bytes of the built-in font data
     */
    static std::span<const char> getData();
};

} // namespace gui
} // namespace bl

#endif
