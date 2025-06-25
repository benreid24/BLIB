#ifndef BLIB_RENDER_UTIL_PERFRAME_HPP
#define BLIB_RENDER_UTIL_PERFRAME_HPP

#include <BLIB/Render/Config/Limits.hpp>
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
     * @brief Construct a new PerFrame object in the empty state
     */
    PerFrame();

    /**
     * @brief Explicit copy constructor
     *
     * @param c The PerFrame object to duplicate
     */
    static PerFrame copy(const PerFrame& c) {
        PerFrame r;
        r.vs   = c.vs;
        r.data = c.data;
        return r;
    }

    /**
     * @brief Construct a new PerFrame object in the empty state for objects with custom
     *        constructors
     *
     * @tparam ...TArgs Argument types to contained object constructors
     * @param ...args Arguments to init each object with
     */
    template<typename... TArgs>
    PerFrame(TArgs&&... args);

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
     * @return  T& The active buffered object for the current frame
     */
    T& current();

    /**
     * @brief Returns the active buffered object for the current frame
     *
     * @return  const T& The active buffered object for the current frame
     */
    const T& current() const;

    /**
     * @brief Returns the next buffered object for the next frame
     *
     * @return  T& The next buffered object for the next frame
     */
    T& next();

    /**
     * @brief Returns the next buffered object for the next frame
     *
     * @return  const T& The next buffered object for the next frame
     */
    const T& next() const;

    /**
     * @brief Helper method to return the current index of the current frame
     */
    std::uint32_t getCurrentIndex() const;

    /**
     * @brief Helper method to get the index of a specific member in this PerFrame
     *
     * @param member The member to get index of. Undefined behavior if not in this PerFrame
     * @return The index of the given member
     */
    std::uint32_t getIndex(const T& member) const;

    /**
     * @brief Helper method to get the corresponding member of a different PerFrame object
     *
     * @tparam U The type contained in the other PerFrame object
     * @param member The member in this object to get the corresponding member of
     * @param other The other PerFrame to get the corresponding member out of
     * @return The member from the other PerFrame with the same index as this one
     */
    template<typename U>
    U& getOther(const T& member, PerFrame<U>& other) const;

    /**
     * @brief Provides direct access to the contained values
     *
     * @param i Index to access
     * @return Object at that index
     */
    T& getRaw(unsigned int i);

    /**
     * @brief Provides direct access to the contained values
     *
     * @param i Index to access
     * @return Object at that index
     */
    const T& getRaw(unsigned int i) const;

    /**
     * @brief Returns whether or not the PerFrame has been initialized yet
     */
    bool valid() const;

    /**
     * @brief Returns the number of contained objects
     */
    std::size_t size() const;

    /**
     * @brief Returns a pointer to the underlying data as an array
     */
    T* rawData();

    /**
     * @brief Returns a pointer to the underlying data as an array
     */
    const T* rawData() const;

private:
    VulkanState* vs;
    std::array<T, cfg::Limits::MaxConcurrentFrames> data;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
