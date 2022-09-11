#ifndef BLIB_ENGINE_RESOURCES_HPP
#define BLIB_ENGINE_RESOURCES_HPP

#include <BLIB/Media/Audio/Playlist.hpp>
#include <BLIB/Media/Graphics/AnimationData.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace bl
{
namespace engine
{
/**
 * @brief Globally available structure containing ResourceManagers for the core resources used by
 *        the engine and almost all applications. Currently all resources are loaded from files.
 *
 * @ingroup Engine
 *
 */
struct Resources : private util::NonCopyable {
    /**
     * @brief Returns a reference to the resource manager for sf::SoundBuffer
     *
     */
    static resource::Manager<sf::SoundBuffer>& sounds();

    /**
     * @brief Returns a reference to the resource manager for audio::Playlist
     *
     */
    static resource::Manager<audio::Playlist>& playlists();

    /**
     * @brief Returns a reference to the resource manager for sf::Texture
     *
     */
    static resource::Manager<sf::Texture>& textures();

    /**
     * @brief Returns a reference to the resource manager for sf::Font
     *
     */
    static resource::Manager<sf::Font>& fonts();

    /**
     * @brief Returns a reference to the resource manager for AnimationData
     *
     */
    static resource::Manager<gfx::AnimationData>& animations();

private:
    resource::Manager<sf::SoundBuffer> _sounds;
    resource::Manager<sf::Texture> _textures;
    resource::Manager<sf::Font> _fonts;
    resource::Manager<gfx::AnimationData> _animations;
    resource::Manager<audio::Playlist> _playlists;

    Resources();
    static Resources& get();
};

} // namespace engine
} // namespace bl

#endif
