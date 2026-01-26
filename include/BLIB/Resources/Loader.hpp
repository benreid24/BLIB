#ifndef BLIB_RESOURCES_RESOURCELOADER_HPP
#define BLIB_RESOURCES_RESOURCELOADER_HPP

#include <BLIB/Audio/Playlist.hpp>
#include <BLIB/Graphics/Animation2D/AnimationData.hpp>
#include <BLIB/Graphics/Text/VulkanFont.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Models/Importer.hpp>
#include <BLIB/Models/Model.hpp>
#include <BLIB/Resources/Ref.hpp>
#include <BLIB/Resources/Resource.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <istream>
#include <string>

namespace bl
{
namespace resource
{
/**
 * @brief Templated resource loader. Must be implemented for any resource type that is
 *        intended to be managed
 *
 * @ingroup Resources
 */
template<typename TResourceType>
struct LoaderBase {
    /**
     * @brief Destroy the Loader Base object
     *
     */
    virtual ~LoaderBase() = default;

    /**
     * @brief Load the resource as identified by the uri
     *
     * @param path Path to the resource to load
     * @param source The raw data of the resource to load
     * @param input istream wrapper around the source buffer
     * @param result The resource object to initialize
     * @return True if the resource could be loaded, false otherwise
     */
    virtual bool load(const std::string& path, const char* buffer, std::size_t len,
                      std::istream& input, TResourceType& result) = 0;
};

/**
 * @brief Helper loader to use if no more of a given resource type should be loaded. Does not
 *        perform any loading and logs warnings when resource loading is attempted
 *
 * @tparam TResourceType The resource type to not load
 * @ingroup Resources
 */
template<typename TResourceType>
struct NullLoader : public LoaderBase<TResourceType> {
    /**
     * @brief Destroy the Null Loader object
     *
     */
    virtual ~NullLoader() = default;

    /**
     * @brief Logs a warning if called
     *
     * @param path Path to the resource to load
     * @return False always
     */
    virtual bool load(const std::string& path, const char*, std::size_t, std::istream&,
                      TResourceType&) override {
        BL_LOG_WARN << "Attempted to load '" << path << " (" << typeid(TResourceType).name()
                    << ") with NullLoader";
        return false;
    }
};

/**
 * @brief Default loaders provided for types: sf::Texture, sf::Image, sf::Font, sf::SoundBuffer,
 *        bl::audio::Playlist, bl::rc::AnimationData, sf::VulkanFont
 *
 * @tparam T One of the above listed types
 * @ingroup Resources
 */
template<typename T>
struct DefaultLoader : public LoaderBase<T> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string&, const char*, std::size_t, std::istream&, T&) override {
        BL_LOG_CRITICAL << "DefaultLoader being used for resource " << typeid(T).name();
        return false;
    }
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<>
struct DefaultLoader<sf::Texture> : public LoaderBase<sf::Texture> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      sf::Texture& result) override {
        if (!result.loadFromMemory(buffer, len)) {
            BL_LOG_ERROR << "Failed to load texture: " << path;
            return false;
        }
        return true;
    }
};

template<>
struct DefaultLoader<sf::SoundBuffer> : public LoaderBase<sf::SoundBuffer> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      sf::SoundBuffer& result) override {
        if (!result.loadFromMemory(buffer, len)) {
            BL_LOG_ERROR << "Failed to load sound: " << path;
            return false;
        }
        return true;
    }
};

template<>
struct DefaultLoader<audio::Playlist> : public LoaderBase<audio::Playlist> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      audio::Playlist& result) override {
        if (!result.loadJson(buffer, len)) {
            BL_LOG_ERROR << "Failed to load playlist: " << path;
            return false;
        }
        return true;
    }
};

using RawPlaylistLoader = DefaultLoader<audio::Playlist>;

struct BundledPlaylistLoader : public LoaderBase<audio::Playlist> {
    virtual ~BundledPlaylistLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      audio::Playlist& result) override {
        if (!result.loadBinary(buffer, len)) {
            BL_LOG_ERROR << "Failed to load playlist: " << path;
            return false;
        }
        return true;
    }
};

template<>
struct DefaultLoader<sf::Font> : public LoaderBase<sf::Font> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      sf::Font& result) override {
        if (!result.openFromMemory(buffer, len)) {
            BL_LOG_ERROR << "Failed to load font: " << path;
            return false;
        }
        return true;
    }
};

template<>
struct DefaultLoader<sf::VulkanFont> : public LoaderBase<sf::VulkanFont> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      sf::VulkanFont& result) override {
        if (!result.loadFromMemory(buffer, len)) {
            BL_LOG_ERROR << "Failed to load font: " << path;
            return false;
        }
        return true;
    }
};

template<>
struct DefaultLoader<sf::Image> : public LoaderBase<sf::Image> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      sf::Image& result) override {
        if (!result.loadFromMemory(buffer, len)) {
            BL_LOG_ERROR << "Failed to load image: " << path;
            return false;
        }
        return true;
    }
};

template<>
struct DefaultLoader<gfx::a2d::AnimationData> : public LoaderBase<gfx::a2d::AnimationData> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      gfx::a2d::AnimationData& result) override {
        if (!result.loadFromMemory(buffer, len, path)) {
            BL_LOG_ERROR << "Failed to load animation: " << path;
            return false;
        }
        return true;
    }
};

template<>
struct DefaultLoader<mdl::Model> : public LoaderBase<mdl::Model> {
    virtual ~DefaultLoader() = default;
    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      mdl::Model& result) override {
        if (!importer.import(buffer, len, result, path)) {
            BL_LOG_ERROR << "Failed to load model: " << path;
            return false;
        }
        return true;
    }

private:
    mdl::Importer importer;
};

} // namespace resource
} // namespace bl

#endif
