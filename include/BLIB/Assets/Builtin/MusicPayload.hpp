#ifndef BLIB_ASSETS_BUILTIN_MUSICPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_MUSICPAYLOAD_HPP

#include <BLIB/Assets/PayloadGeneric.hpp>
#include <SFML/Audio/Music.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Payload for sf::Music
 *
 * @ingroup Assets
 */
using MusicPayload = as::PayloadGeneric<sf::Music>;

} // namespace asi
} // namespace bl

#endif
