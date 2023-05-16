#ifndef BLIB_RENDER_UTIL_PERSWAPFRAME_HPP
#define BLIB_RENDER_UTIL_PERSWAPFRAME_HPP

#include <vector>

namespace bl
{
namespace render
{
class Swapchain;

namespace vk
{
/**
 * @brief Utility class to manage objects where one instance is needed per image in the swap chain.
 *        Provides init and cleanup helpers as well as an accessor for the current object based on
 *        the current swap chain index
 *
 * @tparam T The type of object to store per swap frame. Must be default-constructible
 * @ingroup Renderer
 */
template<typename T>
class PerSwapFrame {
public:
    /**
     * @brief Creates an empty structure
     *
     */
    PerSwapFrame();

    /**
     * @brief Initializes the contained objects. Clears the underlying storage before creating new
     *        objects
     *
     * @tparam TCb Signature of the callback to invoke per object. Should have signature void(T&)
     * @param swapchain The swapchain to pair with
     * @param visitor The visitor to initialize contained objects with
     */
    template<typename TCb>
    void init(Swapchain& swapchain, const TCb& visitor);

    /**
     * @brief Runs a cleanup visitor per-object
     *
     * @tparam TCb Visitor signature
     * @param visitor Cleanup visitor. Must have signature void(T&)
     */
    template<typename TCb>
    void cleanup(const TCb& visitor);

    /**
     * @brief Returns a reference to the currently active object
     *
     * @return The currently active object
     */
    constexpr T& current();

    /**
     * @brief Returns a reference to the currently active object
     *
     * @return The currently active object
     */
    constexpr const T& current() const;

private:
    Swapchain* chain;
    std::vector<T> data;
};

} // namespace vk
} // namespace render
} // namespace bl

#endif