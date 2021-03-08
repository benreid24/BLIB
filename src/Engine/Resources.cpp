#include <BLIB/Engine/Resources.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
namespace
{
struct SoundLoader : public resource::Loader<sf::SoundBuffer> {
    virtual ~SoundLoader() = default;

    virtual resource::Resource<sf::SoundBuffer>::Ref load(const std::string& uri) override {
        resource::Resource<sf::SoundBuffer>::Ref ref(new sf::SoundBuffer());
        if (!ref->loadFromFile(uri)) { BL_LOG_ERROR << "Failed to open sound file: " << uri; }
        return ref;
    }
} soundLoader;

struct TextureLoader : public resource::Loader<sf::Texture> {
    virtual ~TextureLoader() = default;

    virtual resource::Resource<sf::Texture>::Ref load(const std::string& uri) override {
        resource::Resource<sf::Texture>::Ref ref(new sf::Texture());
        if (!ref->loadFromFile(uri)) {
            BL_LOG_ERROR << "Failed to load texture from file: " << uri;
        }
        return ref;
    }
} textureLoader;

struct FontLoader : public resource::Loader<sf::Font> {
    virtual ~FontLoader() = default;

    virtual resource::Resource<sf::Font>::Ref load(const std::string& uri) override {
        resource::Resource<sf::Font>::Ref ref(new sf::Font());
        if (!ref->loadFromFile(uri)) { BL_LOG_ERROR << "Failed to load font from file: " << uri; }
        return ref;
    }
} fontLoader;

struct AnimationLoader : public resource::Loader<gfx::AnimationData> {
    virtual ~AnimationLoader() = default;

    virtual resource::Resource<gfx::AnimationData>::Ref load(const std::string& uri) override {
        return resource::Resource<gfx::AnimationData>::Ref(new gfx::AnimationData(uri));
    }
} animationLoader;

} // namespace

Resources::Resources()
: _sounds(soundLoader)
, _textures(textureLoader)
, _fonts(fontLoader)
, _animations(animationLoader) {}

resource::Manager<sf::Texture>& Resources::textures() { return get()._textures; }

resource::Manager<sf::Font>& Resources::fonts() { return get()._fonts; }

resource::Manager<sf::SoundBuffer>& Resources::sounds() { return get()._sounds; }

resource::Manager<gfx::AnimationData>& Resources::animations() { return get()._animations; }

Resources& Resources::get() {
    static Resources resources;
    return resources;
}

} // namespace engine
} // namespace bl
