#ifndef BLIB_RESOURCES_SPECIALIZEDMANAGERS_HPP
#define BLIB_RESOURCES_SPECIALIZEDMANAGERS_HPP

#include <BLIB/Resources/Loader.hpp>
#include <BLIB/Resources/Manager.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace bl
{
namespace resource
{
/**
 * @brief Specialized Loader for sf::Texture
 *
 * @ingroup Resources
 */
struct TextureResourceLoader : public Loader<sf::Texture> {
    virtual Resource<sf::Texture>::Ref load(const std::string& uri) override {
        Resource<sf::Texture>::Ref txtr = std::make_shared<sf::Texture>();
        txtr->loadFromFile(uri);
        return txtr;
    }
};

/**
 * @brief Specialized Loader for sf::Font
 *
 * @ingroup Resources
 *
 */
struct FontResourceLoader : public Loader<sf::Font> {
    virtual Resource<sf::Font>::Ref load(const std::string& uri) override {
        Resource<sf::Font>::Ref font = std::make_shared<sf::Font>();
        font->loadFromFile(uri);
        return font;
    }
};

// TODO - sound. music. more?

/// Specialized Manager for sf::Texture objects
typedef Manager<sf::Texture> TextureResourceManager;

// Specialized Manager for sf::Font objects
typedef Manager<sf::Font> FontResourceManager;

} // namespace resource
} // namespace bl

#endif
