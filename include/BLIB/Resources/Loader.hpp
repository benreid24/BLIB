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
    /**
     * @brief Destroy the Loader Base object
     *
     */
    virtual ~LoaderBase() = default;

    /**
     * @brief Creates a new resource object to be managed. Uses default constructor. Override this
     *        if your type is not default constructable
     *
     * @return Resource<TResourceType>::Ref The newly created resource
     */
    virtual typename Resource<TResourceType>::Ref createEmpty() {
        return std::make_shared<TResourceType>();
    }

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
 * @brief Default loaders provided for types: sf::Texture, sf::Image, sf::Font, sf::SoundBuffer,
 *        bl::audio::Playlist, bl::gfx::AnimationData
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

#ifndef BLIB_TEST_TARGET
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
#endif

template<>
struct DefaultLoader<sf::Font> : public LoaderBase<sf::Font> {
    virtual ~DefaultLoader() = default;

    virtual Resource<sf::Font>::Ref createEmpty() override {
        std::vector<char>* bufCopy = new std::vector<char>();
        Resource<sf::Font>::Ref font(new sf::Font(), FontDeleter{bufCopy});
        bufferMap.emplace(font.get(), bufCopy);
        return font;
    }

    virtual bool load(const std::string& path, const char* buffer, std::size_t len, std::istream&,
                      sf::Font& result) override {
        const auto it = bufferMap.find(&result);
        if (it == bufferMap.end()) {
            BL_LOG_ERROR << "Could not find buffer for font '" << path << "'";
            return false;
        }
        std::vector<char>* bufCopy = it->second;
        bufferMap.erase(it);
        bufCopy->resize(len);
        std::memcpy(bufCopy->data(), buffer, len);
        if (!result.loadFromMemory(bufCopy->data(), len)) {
            BL_LOG_ERROR << "Failed to load font: " << path;
            return false;
        }
        return true;
    }

private:
    std::unordered_map<sf::Font*, std::vector<char>*> bufferMap;

    struct FontDeleter {
        std::vector<char>* buffer;
        void operator()(sf::Font* font) {
            delete font;
            delete buffer;
        }
    };
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
