#ifndef BLIB_RESOURCES_RESOURCELOADER_HPP
#define BLIB_RESOURCES_RESOURCELOADER_HPP

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <memory>
#include <string>

namespace bl
{
/**
 * @brief Convenience typedef for resource references
 */
template<typename T>
struct Resource {
    typedef std::shared_ptr<T> Ref;
};

/**
 * @brief Templatized resource loader for compile time specification.
 * @brief The default behavior is to attempt to call a constructor that takes the resource URI
 * as a string
 * @brief Resources with different behavior will require specialized loaders
 *
 * @ingroup Resources
 */
template<typename T>
struct ResourceLoader {
    typename Resource<T>::Ref operator()(const std::string& uri) {
        return std::make_shared<T>(uri);
    }
};

/**
 * @brief Specialized ResourceLoader for sf::Texture to load from files
 *
 * @ingroup Resources
 */
struct TextureResourceLoader {
    Resource<sf::Texture>::Ref operator()(const std::string& uri) {
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
struct FontResourceLoader {
    Resource<sf::Font>::Ref operator()(const std::string& uri) {
        Resource<sf::Font>::Ref font = std::make_shared<sf::Font>();
        font->loadFromFile(uri);
        return font;
    }
};

} // namespace bl

#endif