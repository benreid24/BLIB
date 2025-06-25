#ifndef BLIB_RENDER_VULKAN_PERFRAMEVECTOR_HPP
#define BLIB_RENDER_VULKAN_PERFRAMEVECTOR_HPP

#include <cstdint>
#include <vector>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Container version of PerFrame. Intended to replace std::vector<PerFrame<T>> when
 *        continuous storage is needed (ie descriptor sets)
 *
 * @tparam T The type of item to store
 * @ingroup Renderer
 */
template<typename T>
class PerFrameVector {
public:
    PerFrameVector();

    /**
     * @brief Initializes the PerFrameVector object without initializing the wrapped values
     *
     * @param vulkanState Renderer Vulkan state
     */
    void emptyInit(VulkanState& vulkanState, std::uint32_t capacity);

    /**
     * @brief Initialize the buffer and each contained object
     *
     * @tparam U The callback type to init with. Signature void(uint32_t, std::uint32_t, T&)
     * @param vulkanState The VulkanState to sync with
     * @param visitor The callback to init each buffered object
     */
    template<typename U>
    void init(VulkanState& vulkanState, std::uint32_t capacity, const U& visitor);

    /**
     * @brief General buffered object visitor intended primarily for cleanup but could be used for
     *        anything else requiring visiting each buffered object
     *
     * @tparam U The callback type. Signature void(uint32_t, std::uint32_t, T&)
     * @param visitor The callback to invoke for each buffered object
     */
    template<typename U>
    void cleanup(const U& visitor);

    /**
     * @brief Returns the active item at the given index
     *
     * @param i The index to get
     * @return The item for the current frame at the given index
     */
    T& current(std::uint32_t i);

    /**
     * @brief Returns the active item at the given index
     *
     * @param i The index to get
     * @return The item for the current frame at the given index
     */
    const T& current(std::uint32_t i) const;

    /**
     * @brief Manually access the item at the given index for the given frame
     *
     * @param i The index to access
     * @param frame The frame to access
     * @return The item for the given index and frame
     */
    T& getRaw(std::uint32_t i, std::uint32_t frame);

    /**
     * @brief Manually access the item at the given index for the given frame
     *
     * @param i The index to access
     * @param frame The frame to access
     * @return The item for the given index and frame
     */
    const T& getRaw(std::uint32_t i, std::uint32_t frame) const;

    /**
     * @brief Returns the number of elements in the vector
     */
    constexpr std::uint32_t size() const;

    /**
     * @brief Returns the total number of elements when accounting for PerFrame logic
     */
    constexpr std::uint32_t totalSize() const;

    /**
     * @brief Returns a pointer to the first element in the vector
     */
    constexpr T* data();

    /**
     * @brief Returns a pointer to the first element in the vector
     */
    constexpr const T* data() const;

private:
    VulkanState* vs;
    std::uint32_t cap;
    std::vector<T> items;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
