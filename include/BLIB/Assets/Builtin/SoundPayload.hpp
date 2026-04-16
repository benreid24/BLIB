#ifndef BLIB_ASSETS_BUILTIN_AUDIOPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_AUDIOPAYLOAD_HPP

#include <BLIB/Assets/PayloadGeneric.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Payload for sf::SoundBuffer
 *
 * @ingroup Assets
 */
using SoundPayload = as::PayloadGeneric<sf::SoundBuffer>;

} // namespace asi
} // namespace bl

#endif
