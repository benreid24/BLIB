#include "Renderer.hpp"
#include "utils/QueueFamilyLocator.hpp"
#include "utils/RenderSwapFrame.hpp"
#include "utils/SwapChainSupportDetails.hpp"
#include <BLIB/Logging.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <iostream>
#include <optional>
#include <set>
#include <unordered_set>

class VulkanSandbox {
public:
    VulkanSandbox(sf::WindowBase& window)
    : window(window)
    , renderer(window) {}

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) { return; }
                else if (event.type == sf::Event::Resized) { renderer.invalidateSwapChain(); }
                else if (event.type == sf::Event::LostFocus) {
                    if (!awaitFocus()) { return; }
                    renderer.invalidateSwapChain();
                }
            }

            renderer.render();

            sf::sleep(sf::milliseconds(16));
        }
    }

private:
    sf::WindowBase& window;
    Renderer renderer;

    bool awaitFocus() {
        sf::Event event;
        while (window.waitEvent(event)) {
            if (event.type == sf::Event::GainedFocus) return true;
            if (event.type == sf::Event::Closed) return false;
        }
        return false;
    }
};

int main() {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Debug);

    sf::WindowBase window(
        sf::VideoMode(1920, 1080, 32), "Vulkan Playground", sf::Style::Close | sf::Style::Titlebar);

    try {
        VulkanSandbox vulcan(window);
        vulcan.run();
    } catch (const std::exception& exc) {
        BL_LOG_ERROR << exc.what();
        return 1;
    }

    return 0;
}
