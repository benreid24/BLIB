#ifndef BLIB_RESOURCES_SPECIALIZEDMANAGERS_HPP
#define BLIB_RESOURCES_SPECIALIZEDMANAGERS_HPP

#include <BLIB/Resources/ResourceLoader.hpp>
#include <BLIB/Resources/ResourceManager.hpp>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace bl
{
/**
 * @brief Specialized ResourceLoader for sf::Texture
 *
 * @ingroup Resources
 */
struct TextureResourceLoader : public ResourceLoader<sf::Texture> {
    virtual Resource<sf::Texture>::Ref load(const std::string& uri) override {
        Resource<sf::Texture>::Ref txtr = std::make_shared<sf::Texture>();
        txtr->loadFromFile(uri);
        return txtr;
    }
};

/**
 * @brief Specialized ResourceLoader for sf::Font
 *
 * @ingroup Resources
 *
 */
struct FontResourceLoader : public ResourceLoader<sf::Font> {
    virtual Resource<sf::Font>::Ref load(const std::string& uri) override {
        Resource<sf::Font>::Ref font = std::make_shared<sf::Font>();
        font->loadFromFile(uri);
        return font;
    }
};

// TODO - sound. music. more?

/// Specialized ResourceManager for sf::Texture objects
typedef ResourceManager<sf::Texture> TextureResourceManager;

// Specialized ResourceManager for sf::Font objects
typedef ResourceManager<sf::Font> FontResourceManager;

} // namespace bl

#endif