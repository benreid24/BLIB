#ifndef BLIB_RENDER_WINDOW_HPP
#define BLIB_RENDER_WINDOW_HPP

#include <BLIB/Render/Vulkan/VkCheck.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Vulkan.hpp>
#include <SFML/Window.hpp>
#include <cstdint>
#include <string>
#include <type_traits>

#ifdef BLIB_MACOS
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#endif

namespace bl
{
namespace rc
{
/// Internal implementation details
namespace priv
{
#ifdef BLIB_MACOS
class GlfwManager {
public:
    static void onWindowCreate() {
        int& c = get().count;

        if (++c == 1) { glfwInit(); }
    }

    static void onWindowDestroy() {
        int& c = get().count;
        if (--c == 0) { glfwTerminate(); }
    }

private:
    int count;

    GlfwManager()
    : count(0) {}

    static GlfwManager& get() {
        static GlfwManager me;
        return me;
    }
};
#endif
} // namespace priv

/**
 * @brief Wrapper over SFML windows. Uses GLFW on Macos for proper Vulkan
 *        surface creation
 *
 * @tparam SfWindow The type of SFML window to use
 * @ingroup Renderer
 */
template<typename SfWindow>
class Window : private util::NonCopyable {
    static_assert(std::is_base_of_v<sf::WindowBase, SfWindow>,
                  "SfWindow should be sf::WindowBase or derived");

public:
    /**
     * @brief Does nothing
     */
    Window();

    /**
     * @brief Closes the window if it is open
     */
    ~Window();

    /**
     * @brief Creates (or recreates) the window
     *
     * @param mode The video mode to create the window with
     * @param title The title to give the window
     * @param style The style of the window
     */
    void create(sf::VideoMode mode, const std::string& title, std::uint32_t style);

    /**
     * @brief Polls an event from the window
     *
     * @param event An event object to populate
     * @return True if an event was fetched, false if none were available
     */
    bool pollEvent(sf::Event& event);

    /**
     * @brief Blocks until an event was able to be polled
     *
     * @param event An event object to populate
     * @return True if an event was fetched, false otherwise
     */
    bool waitEvent(sf::Event& event);

    /**
     * @brief Closes the window
     */
    void close();

    /**
     * @brief Returns whether or not the window is currently open
     */
    bool isOpen() const;

    /**
     * @brief Returns the underlying SFML window. Do not use methods that are also provided here
     */
    constexpr SfWindow& getSfWindow();

    /**
     * @brief Returns the underlying SFML window. Do not use methods that are also provided here
     */
    constexpr const SfWindow& getSfWindow() const;

    /**
     * @brief Creates a Vulkan surface for the window
     *
     * @param instance The Vulkan instance
     * @param surface The surface handle to populate
     * @return True if the surface could be created, false on error
     */
    bool createVulkanSurface(VkInstance instance, VkSurfaceKHR& surface);

    /**
     * @brief Returns the required Vulkan instance extensions
     */
    static std::vector<const char*> getRequiredInstanceExtensions();

private:
    SfWindow sfWindow;
#ifdef BLIB_MACOS
    GLFWwindow* glfwWindow;
#endif
};

/**
 * @brief Default window type used by the renderer
 *
 * @ingroup Renderer
 */
using RenderWindow = Window<sf::WindowBase>;

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Window<T>::Window()
#ifdef BLIB_MACOS
: glfwWindow(nullptr)
#endif
{
}

template<typename T>
Window<T>::~Window() {
    close();
}

template<typename T>
void Window<T>::create(sf::VideoMode mode, const std::string& title, std::uint32_t style) {
    close();

#ifdef BLIB_MACOS
    priv::GlfwManager::onWindowCreate();

    const auto colorBits = mode.bitsPerPixel / 4;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RED_BITS, colorBits);
    glfwWindowHint(GLFW_GREEN_BITS, colorBits);
    glfwWindowHint(GLFW_BLUE_BITS, colorBits);
    glfwWindowHint(GLFW_ALPHA_BITS, colorBits);
    glfwWindowHint(GLFW_RESIZABLE, (style & sf::Style::Resize) != 0 ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED,
                   (style & (sf::Style::Close | sf::Style::Titlebar)) != 0 ? GLFW_TRUE :
                                                                             GLFW_FALSE);
    GLFWmonitor* monitor = (style & sf::Style::Fullscreen) != 0 ? glfwGetPrimaryMonitor() : nullptr;

    glfwWindow = glfwCreateWindow(mode.width, mode.height, title.c_str(), monitor, nullptr);
    if (!glfwWindow) return;
    sfWindow.create(glfwGetCocoaWindow(glfwWindow));
#else
    sfWindow.create(mode, title, style);
#endif
}

template<typename T>
bool Window<T>::pollEvent(sf::Event& event) {
    if (!isOpen()) return false;

#ifdef BLIB_MACOS
    if (glfwWindowShouldClose(glfwWindow)) {
        event.type = sf::Event::Closed;
        return true;
    }
#endif
    return sfWindow.pollEvent(event);
}

template<typename T>
bool Window<T>::waitEvent(sf::Event& event) {
    if (!isOpen()) return false;
    while (!pollEvent(event)) { sf::sleep(sf::milliseconds(30)); }
    return true;
}

template<typename T>
void Window<T>::close() {
#ifdef BLIB_MACOS
    if (glfwWindow) {
        glfwDestroyWindow(glfwWindow);
        glfwWindow = nullptr;
        priv::GlfwManager::onWindowDestroy();
    }
#else
    sfWindow.close();
#endif
}

template<typename T>
constexpr T& Window<T>::getSfWindow() {
    return sfWindow;
}

template<typename T>
constexpr const T& Window<T>::getSfWindow() const {
    return sfWindow;
}

template<typename T>
bool Window<T>::isOpen() const {
#ifdef BLIB_MACOS
    return glfwWindow != nullptr;
#else
    return sfWindow.isOpen();
#endif
}
template<typename T>
bool Window<T>::createVulkanSurface(VkInstance instance, VkSurfaceKHR& surface) {
#ifdef BLIB_MACOS
    // TODO - export MVK_CONFIG_USE_METAL_ARGUMENT_BUFFERS=1
    // https://github.com/KhronosGroup/MoltenVK/tree/main#hiding-vulkan-api-symbols
    return VK_SUCCESS == glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);
#else
    return sfWindow.createVulkanSurface(instance, surface);
#endif
}

template<typename T>
std::vector<const char*> Window<T>::getRequiredInstanceExtensions() {
#ifdef BLIB_MACOS
    std::uint32_t extensionCount = 0;
    const char** exts            = glfwGetRequiredInstanceExtensions(&extensionCount);
    std::vector<const char*> result;
    result.resize(extensionCount + 1);
    for (std::uint32_t i = 0; i < extensionCount; ++i) { result[i] = exts[i]; }
    result.back() = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
    return result;
#else
    return sf::Vulkan::getGraphicsRequiredInstanceExtensions();
#endif
}

} // namespace rc
} // namespace bl

#endif
