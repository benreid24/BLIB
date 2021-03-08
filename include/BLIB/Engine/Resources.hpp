#ifndef BLIB_ENGINE_RESOURCES_HPP
#define BLIB_ENGINE_RESOURCES_HPP

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
    static ResourceManager<sf::SoundBuffer>& sounds();

    /**
     * @brief Returns a reference to the resource manager for sf::Texture
     *
     */
    static ResourceManager<sf::Texture>& textures();

    /**
     * @brief Returns a reference to the resource manager for sf::Font
     *
     */
    static ResourceManager<sf::Font>& fonts();

    /**
     * @brief Returns a reference to the resource manager for AnimationData
     *
     */
    static ResourceManager<AnimationData>& animations();

private:
    ResourceManager<sf::SoundBuffer> _sounds;
    ResourceManager<sf::Texture> _textures;
    ResourceManager<sf::Font> _fonts;
    ResourceManager<AnimationData> _animations;

    Resources();
    static Resources& get();
};

} // namespace engine
} // namespace bl

#endif
