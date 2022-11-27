#include <BLIB/Resources.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

// Provide a callback for BackgroundLoader to report progress to
// This can be any method, including a method in a class with plenty of state information
void progressCallback(float progress) { std::cout << "Loading: " << (progress * 100.f) << "%\n"; }

int main() {
    // Load a managed texture using the new resource manager
    bl::resource::Resource<sf::Texture> texture =
        bl::resource::ResourceManager<sf::Texture>::load("image.png");

    return 0;
}
