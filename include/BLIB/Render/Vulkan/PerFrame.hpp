#ifndef BLIB_RENDER_UTIL_PERFRAME_HPP
#define BLIB_RENDER_UTIL_PERFRAME_HPP

#include <BLIB/Render/Config.hpp>
#include <array>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Utility struct to easily double-buffer objects and data on a per-frame basis. One object
 *        of type T will exists per concurrent frame and the current will be accessible
 *
 * @tparam T The type of object to buffer per-frame
 * @ingroup Renderer
 */
template<typename T>
class PerFrame {
public:
    /**
     * @brief Construct a new Per Frame object in the empty state
     *
     */
    PerFrame();

    /**
     * @brief Initializes the PerFrame object without initializing the wrapped values
     *
     * @param vulkanState Renderer Vulkan state
     */
    void emptyInit(VulkanState& vulkanState);

    /**
     * @brief Initialize the buffer and each contained object
     *
     * @tparam U The callback type to init each object with
     * @param vulkanState The VulkanState to sync with
     * @param visitor The callback to init each buffered object
     */
    template<typename U>
    void init(VulkanState& vulkanState, const U& visitor);

    /**
     * @brief General buffered object visitor intended primarily for cleanup but could be used for
     *        anything else requiring visiting each buffered object
     *
     * @tparam U The callback type
     * @param visitor The callback to invoke for each buffered object
     */
    template<typename U>
    void cleanup(const U& visitor);

    /**
     * @brief General buffered object visitor
     *
     * @tparam U The callback type
     * @param visitor The callback to invoke for each buffered object
     */
    template<typename U>
    void visit(const U& visitor);

    /**
     * @brief Returns the active buffered object for the current frame
     *
     * @return constexpr T& The active buffered object for the current frame
     */
    constexpr T& current();

    /**
     * @brief Returns the active buffered object for the current frame
     *
     * @return constexpr const T& The active buffered object for the current frame
     */
    constexpr const T& current() const;

    /**
     * @brief Provides direct access to the contained values
     *
     * @param i Index to access
     * @return Object at that index
     */
    constexpr T& getRaw(unsigned int i);

    /**
     * @brief Provides direct access to the contained values
     *
     * @param i Index to access
     * @return Object at that index
     */
    constexpr const T& getRaw(unsigned int i) const;

    /**
     * @brief Returns whether or not the PerFrame has been initialized yet
     */
    constexpr bool valid() const;

    /**
     * @brief Returns the number of contained objects
     */
    constexpr std::size_t size() const;

    /**
     * @brief Returns a pointer to the underlying data as an array
     */
    constexpr T* rawData();

    /**
     * @brief Returns a pointer to the underlying data as an array
     */
    constexpr const T* rawData() const;

private:
    VulkanState* vs;
    std::array<T, Config::MaxConcurrentFrames> data;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
