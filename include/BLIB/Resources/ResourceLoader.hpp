#ifndef BLIB_RESOURCES_RESOURCELOADER_HPP
#define BLIB_RESOURCES_RESOURCELOADER_HPP

#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <memory>

namespace bl
{

/**
 * Convenience typedef for resource references
 */
template<typename T>
struct Resource {
    typedef std::shared_ptr<T> Ref;
};

/**
 * Templatized resource loader for compile time specification. 
 * The default behavior is to attempt to call a constructor that takes the resource URI as a string
 * Resources with different behavior will require specialized loaders
 * 
 * \ingroup Resources
 */
template<typename T>
struct ResourceLoader {
    typename Resource<T>::Ref operator() (const std::string& uri) {
        return std::make_shared<T>(uri);
    }
};

/**
 * Specialized ResourceLoader for sf::Texture to load from files
 * 
 * \ingroup Resources
 */
struct TextureFileLoader {
    Resource<sf::Texture>::Ref operator() (const std::string& uri) {
        Resource<sf::Texture>::Ref txtr = std::make_shared<sf::Texture>();
        txtr->loadFromFile(uri);
        return txtr;
    }
};

}

#endif