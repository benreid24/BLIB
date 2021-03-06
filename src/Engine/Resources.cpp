#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
namespace
{
struct SoundLoader : public ResourceLoader<sf::SoundBuffer> {
    virtual ~SoundLoader() = default;

    virtual Resource<sf::SoundBuffer>::Ref load(const std::string& uri) override {
        Resource<sf::SoundBuffer>::Ref ref(new sf::SoundBuffer());
        if (!ref->loadFromFile(uri)) { BL_LOG_ERROR << "Failed to open sound file: " << uri; }
        return ref;
    }
} soundLoader;

struct TextureLoader : public ResourceLoader<sf::Texture> {
    virtual ~TextureLoader() = default;

    virtual Resource<sf::Texture>::Ref load(const std::string& uri) override {
        Resource<sf::Texture>::Ref ref(new sf::Texture());
        if (!ref->loadFromFile(uri)) {
            BL_LOG_ERROR << "Failed to load texture from file: " << uri;
        }
        return ref;
    }
} textureLoader;

struct FontLoader : public ResourceLoader<sf::Font> {
    virtual ~FontLoader() = default;

    virtual Resource<sf::Font>::Ref load(const std::string& uri) override {
        Resource<sf::Font>::Ref ref(new sf::Font());
        if (!ref->loadFromFile(uri)) { BL_LOG_ERROR << "Failed to load font from file: " << uri; }
        return ref;
    }
} fontLoader;

struct AnimationLoader : public ResourceLoader<AnimationData> {
    virtual ~AnimationLoader() = default;

    virtual Resource<AnimationData>::Ref load(const std::string& uri) override {
        return Resource<AnimationData>::Ref(new AnimationData(uri, spritesheetPath));
    }

    std::string spritesheetPath;
} animationLoader;

} // namespace

Resources::Resources()
: _sounds(soundLoader)
, _textures(textureLoader)
, _fonts(fontLoader)
, _animations(animationLoader) {}

ResourceManager<sf::Texture>& Resources::textures() { return get()._textures; }

ResourceManager<sf::Font>& Resources::fonts() { return get()._fonts; }

ResourceManager<sf::SoundBuffer>& Resources::sounds() { return get()._sounds; }

void Resources::setSpritesheetPath(const std::string& p) { animationLoader.spritesheetPath = p; }

Resources& Resources::get() {
    static Resources resources;
    return resources;
}

} // namespace engine
} // namespace bl
