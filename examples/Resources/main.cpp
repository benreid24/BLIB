#include "CharacterHandler.hpp"
#include "CharacterLoaders.hpp"
#include "MapHandler.hpp"
#include "MapLoaders.hpp"
#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>

using namespace bl::resource;

const std::string BundlePath = "bundles";

void createBundles() {
    Bundler bundler(
        bundle::Config(BundlePath)
            .addBundleSource({"game_data/maps", bundle::BundleSource::CreateBundleForEachContained})
            .addBundleSource({"game_data/audio", bundle::BundleSource::BundleAllFiles})
            .addBundleSource({"game_data/images", bundle::BundleSource::BundleAllFiles})
            .addExcludePattern(".*\\.temp")
            .addFileHandler<MapHandler>(".*/maps/.*\\.json")
            .addFileHandler<CharacterHandler>(".*/characters/.*\\.json")
            .withCatchAllDirectory("game_data")
            .build());

    if (!bundler.createBundles()) {
        BL_LOG_ERROR << "Failed to create bundles!";
        std::exit(1);
    }
}

void useBundles() {
    if (!FileSystem::useBundle(BundlePath)) {
        BL_LOG_CRITICAL << "Failed to attach bundles!";
        std::exit(1);
    }

    // in release mode we want to use the proper loaders
    ResourceManager<Character>::installLoader<CharacterLoaderProdMode>();
    ResourceManager<Map>::installLoader<MapLoaderProdMode>();
}

int main() {
    // Load a managed texture from disk (default state)
    Resource<sf::Texture> texture =
        ResourceManager<sf::Texture>::load("game_data/images/image.png");

    // use custom loaders for our custom types
    ResourceManager<Character>::installLoader<CharacterLoaderDevMode>();
    ResourceManager<Map>::installLoader<MapLoaderDevMode>();

    // load json resources from disk
    Resource<Character>::Ref npc =
        ResourceManager<Character>::load("game_data/characters/npc1.json").data;
    BL_LOG_INFO << "NPC name from disk: " << npc->name;

    // create bundles from from our raw resources
    createBundles();

    // in release mode we want to use our bundles and release loaders
    useBundles();

    // load a resource from a bundle
    Resource<Map>::Ref map = ResourceManager<Map>::load("game_data/maps/map.json").data;
    BL_LOG_INFO << "Map name from bundle: " << map->name;

    return 0;
}
