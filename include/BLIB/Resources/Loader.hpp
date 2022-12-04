#ifndef BLIB_RESOURCES_RESOURCELOADER_HPP
#define BLIB_RESOURCES_RESOURCELOADER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Media/Graphics/AnimationData.hpp>
#include <BLIB/Resources/Resource.hpp>
#include <SFML/Graphics.hpp>
#include <istream>
#include <string>

#ifndef BLIB_TEST_TARGGET
#include <BLIB/Media/Audio/Playlist.hpp>
#include <SFML/Audio.hpp>
#endif

namespace bl
{
namespace resource
{
/**
 * @brief Templatized resource loader. Must be implemented for any resource type that is
 *        intended to be managed
 *
 * @ingroup Resources
 */
template<typename TResourceType>
struct LoaderBase {
    virtual ~LoaderBase() = default;

    /**
     * @brief Load the resource as identified by the uri
     *
     * @param path Path to the resource to load
     * @param source The raw data of the resource to load
     * @param input istream wrapper around the source buffer
     * @return Resource<TResourceType>::Ref The loaded resource
     */
    virtual typename Resource<TResourceType>::Ref load(const std::string& path,
                                                       const std::vector<char>& source,
                                                       std::istream& input) = 0;
};

/**
 * @brief Default loaders provided for types: sf::Texture, sf::Image, sf::Font, sf::SoundBuffer,
 *        bl::audio::Playlist, bl::gfx::AnimationData
 *
 * @tparam T One of the above listed types
 * @ingroup Resources
 */
template<typename T>
struct DefaultLoader : public LoaderBase<T> {
    virtual ~DefaultLoader() = default;

    virtual typename Resource<T>::Ref load(const std::string&, const std::vector<char>&,
                                           std::istream&) override {
        BL_LOG_CRITICAL << "DefaultLoader being used for resource " << typeid(T).name();
        return {};
    }
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<>
struct DefaultLoader<sf::Texture> : public LoaderBase<sf::Texture> {
    virtual ~DefaultLoader() = default;

    virtual Resource<sf::Texture>::Ref load(const std::string& path, const std::vector<char>& data,
                                            std::istream&) override {
        Resource<sf::Texture>::Ref res = std::make_shared<sf::Texture>();
        if (!res->loadFromMemory(data.data(), data.size())) {
            BL_LOG_ERROR << "Failed to load texture: " << path;
        }
        return res;
    }
};

#ifndef BLIB_TEST_TARGET
template<>
struct DefaultLoader<sf::SoundBuffer> : public LoaderBase<sf::SoundBuffer> {
    virtual ~DefaultLoader() = default;

    virtual Resource<sf::SoundBuffer>::Ref load(const std::string& path,
                                                const std::vector<char>& data,
                                                std::istream&) override {
        Resource<sf::SoundBuffer>::Ref res = std::make_shared<sf::SoundBuffer>();
        if (!res->loadFromMemory(data.data(), data.size())) {
            BL_LOG_ERROR << "Failed to load sound: " << path;
        }
        return res;
    }
};

template<>
struct DefaultLoader<audio::Playlist> : public LoaderBase<audio::Playlist> {
    virtual ~DefaultLoader() = default;

    virtual Resource<audio::Playlist>::Ref load(const std::string& path,
                                                const std::vector<char>& data,
                                                std::istream&) override {
        Resource<audio::Playlist>::Ref res = std::make_shared<audio::Playlist>();
        if (!res->loadFromMemory(data)) { BL_LOG_ERROR << "Failed to load playlist: " << path; }
        return res;
    }
};
#endif

template<>
struct DefaultLoader<sf::Font> : public LoaderBase<sf::Font> {
    virtual ~DefaultLoader() = default;

    virtual Resource<sf::Font>::Ref load(const std::string& path, const std::vector<char>& data,
                                         std::istream&) override {
        Resource<sf::Font>::Ref res = std::make_shared<sf::Font>();
        if (!res->loadFromMemory(data.data(), data.size())) {
            BL_LOG_ERROR << "Failed to load font: " << path;
        }
        return res;
    }
};

template<>
struct DefaultLoader<sf::Image> : public LoaderBase<sf::Image> {
    virtual ~DefaultLoader() = default;

    virtual Resource<sf::Image>::Ref load(const std::string& path, const std::vector<char>& data,
                                          std::istream&) override {
        Resource<sf::Image>::Ref res = std::make_shared<sf::Image>();
        if (!res->loadFromMemory(data.data(), data.size())) {
            BL_LOG_ERROR << "Failed to load image: " << path;
        }
        return res;
    }
};

template<>
struct DefaultLoader<gfx::AnimationData> : public LoaderBase<gfx::AnimationData> {
    virtual ~DefaultLoader() = default;

    virtual Resource<gfx::AnimationData>::Ref load(const std::string& path,
                                                   const std::vector<char>& data,
                                                   std::istream&) override {
        Resource<gfx::AnimationData>::Ref res = std::make_shared<gfx::AnimationData>();
        if (!res->loadFromMemory(data, path)) {
            BL_LOG_ERROR << "Failed to load animation: " << path;
        }
        return res;
    }
};

} // namespace resource
} // namespace bl

#endif
