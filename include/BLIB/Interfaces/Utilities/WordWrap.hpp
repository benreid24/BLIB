#ifndef BLIB_INTERFACES_UTILITIES_WORDWRAP_HPP
#define BLIB_INTERFACES_UTILITIES_WORDWRAP_HPP

#include <SFML/Graphics.hpp>

namespace bl
{
namespace interface
{
/**
 * @brief Basic helper function to wrap text to fit within a specified maximum width
 *
 * @param text The text to wrap. Must be initialized with font and size. Modifed in place
 * @param maxWidth The width to constrain the text to
 *
 * @ingroup InterfaceUtilities
 */
void wordWrap(sf::Text& text, float maxWidth);

} // namespace interface
} // namespace bl

#endif
