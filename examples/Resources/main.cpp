#include "CharacterHandler.hpp"
#include "MapHandler.hpp"
#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>

using namespace bl::resource;

void createBundles() {
    Bundler bundler(
        bundle::Config("bundles")
            .addBundleSource({"game_data/maps", bundle::BundleSource::CreateBundleForEachContained})
            .addBundleSource({"game_data/audio", bundle::BundleSource::BundleAllFiles})
            .addBundleSource({"game_data/images", bundle::BundleSource::BundleAllFiles})
            .addExcludePattern(".*\\.temp")
            .addFileHandler<MapHandler>(".*/maps/.*\\.json")
            .addFileHandler<CharacterHandler>(".*/characters/.*\\.json")
            .withCatchAllDirectory("game_data"));

    if (!bundler.createBundles()) {
        BL_LOG_ERROR << "Failed to create bundles!";
        std::exit(1);
    }
}

int main() {
    // Load a managed texture from disk (default state)
    Resource<sf::Texture> texture = ResourceManager<sf::Texture>::load("game_dataimage.png");

    return 0;
}
