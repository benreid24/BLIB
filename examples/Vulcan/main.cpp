#include "Renderer.hpp"
#include "utils/IndexBuffer.hpp"
#include "utils/QueueFamilyLocator.hpp"
#include "utils/RenderSwapFrame.hpp"
#include "utils/SwapChainSupportDetails.hpp"
#include "utils/Vertex.hpp"
#include "utils/VertexBuffer.hpp"
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

const std::vector<Vertex> vertices = {{{0.0f, -0.5f}, {0.3f, 0.8f, 0.8f}},
                                      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

const std::vector<Vertex> ibufVertices   = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
const std::vector<std::uint32_t> indices = {0, 1, 2, 2, 3, 0};

class VulkanSandbox {
public:
    VulkanSandbox(sf::WindowBase& window)
    : window(window)
    , renderer(window)
    , vertexBuffer(renderer, vertices.size())
    , indexBuffer(renderer, ibufVertices.size(), indices.size()) {
        std::memcpy(vertexBuffer.data(), vertices.data(), sizeof(Vertex) * vertices.size());
        vertexBuffer.sendToGPU();

        std::memcpy(indexBuffer.data(), ibufVertices.data(), sizeof(Vertex) * ibufVertices.size());
        std::memcpy(
            indexBuffer.indiceData(), indices.data(), sizeof(std::uint32_t) * indices.size());
        indexBuffer.sendToGPU();
    }

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

            const auto doRender = [this](VkCommandBuffer commandBuffer) {
                vertexBuffer.render(commandBuffer);
                indexBuffer.render(commandBuffer);
            };

            renderer.render(doRender);

            sf::sleep(sf::milliseconds(16));
        }
    }

private:
    sf::WindowBase& window;
    Renderer renderer;

    VertexBuffer<Vertex> vertexBuffer;
    IndexBuffer<Vertex> indexBuffer;

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
