#ifndef BLIB_RESOURCES_RESOURCELOADER_HPP
#define BLIB_RESOURCES_RESOURCELOADER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Media/Audio/Playlist.hpp>
#include <BLIB/Media/Graphics/AnimationData.hpp>
#include <BLIB/Render/Drawables/Text/VulkanFont.hpp>
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
 * @brief Templatized resource loader. Must be implemented for any resource type that is
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
 *        bl::audio::Playlist, bl::gfx::AnimationData, sf::VulkanFont
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
#ifndef BLIB_HEADLESS_FOR_CI_TESTING
        if (!result.loadFromMemory(buffer, len)) {
            BL_LOG_ERROR << "Failed to load texture: " << path;
            return false;
        }
#endif
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
        if (!result.loadFromMemory(buffer, len)) {
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
struct DefaultLoader<gfx::AnimationData> : public LoaderBase<gfx::AnimationData> {
    virtual ~DefaultLoader() = default;

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      gfx::AnimationData& result) override {
        if (!result.loadFromMemory(buffer, len, path)) {
            BL_LOG_ERROR << "Failed to load animation: " << path;
            return false;
        }
        return true;
    }
};

} // namespace resource
} // namespace bl

#endif
