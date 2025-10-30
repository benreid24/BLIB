#ifndef BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFER_HPP
#define BLIB_RENDER_BUFFERS_INTERFACES_BINDABLEBUFFER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Buffers/Alignment.hpp>
#include <BLIB/Render/Buffers/DirtyRange.hpp>
#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Vulkan.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;
class Buffer;
} // namespace vk

namespace buf
{
/// Namespace containing base buffer classes that provide interfaces
namespace base
{
/**
 * @brief Base class for buffers that are intended for use as SSBO's or uniforms
 *
 * @tparam T The payload type that the buffer will hold
 * @ingroup Renderer
 */
template<typename T, Alignment Align>
class BindableBuffer : public tfr::Transferable {
public:
    /**
     * @brief Creates the buffer wrapper
     */
    BindableBuffer()
    : alignment(sizeof(T))
    , dirtyRanges{}
    , currentDirtyRange(0)
    , copyFullRange(false) {}

    /**
     * @brief Destroys the buffer wrapper
     */
    virtual ~BindableBuffer() = default;

    /**
     * @brief Creates the buffer
     *
     * @param vs The renderer Vulkan state
     * @param numElements The number of data elements to size the buffer for
     */
    void create(vk::VulkanState& vs, std::uint32_t numElements) {
        vulkanState = &vs;
        alignment   = computeAlignment(sizeof(T), Align);
        doCreate(vs, numElements);
    }

    /**
     * @brief Immediately destroys the buffer
     */
    virtual void destroy() = 0;

    /**
     * @brief Queues the buffer to be destroyed after MaxConcurrentFrames
     */
    virtual void deferDestroy() = 0;

    /**
     * @brief Resizes the buffer
     *
     * @param size The new number of data elements to size for
     */
    virtual void resize(std::uint32_t size) = 0;

    /**
     * @brief If the buffer is smaller resizes to contain at least the given number of elements
     *
     * @param size The number of elements to ensure storage for
     * @return True if the buffer resized, false if it was already large enough
     */
    bool ensureSize(std::uint32_t size) {
        if (size > getSize()) {
            std::uint32_t newSize = std::max(getSize(), static_cast<std::uint32_t>(1)) * 2;
            while (newSize < size) { newSize *= 2; }
            resize(newSize);
            return true;
        }
        return false;
    }

    /**
     * @brief Returns the number of data elements in the buffer
     */
    virtual std::uint32_t getSize() const = 0;

    /**
     * @brief Returns the size, in bytes, of a single data element plus padding for alignment
     */
    std::uint32_t getAlignedElementSize() const { return alignment; }

    /**
     * @brief Returns the total size, in bytes, of the buffer
     */
    std::size_t getTotalAlignedSize() const { return getSize() * getAlignedElementSize(); }

    /**
     * @brief Returns the underlying buffer for the given frame index
     *
     * @param frameIndex The frame index to get the buffer for
     */
    virtual vk::Buffer& getBuffer(std::uint32_t frameIndex) = 0;

    /**
     * @brief Returns the underlying buffer for the current frame
     */
    vk::Buffer& getCurrentFrameBuffer() { return getBuffer(vulkanState->currentFrameIndex()); }

    /**
     * @brief Returns the direct Vulkan handle of the underlying buffer for the given frame
     *
     * @param frameIndex The index of the frame to get the buffer handle for
     * @return The Vulkan buffer handle for the given frame
     */
    VkBuffer getRawBuffer(std::uint32_t frameIndex) { return getBuffer(frameIndex).getBuffer(); }

    /**
     * @brief Returns the direct Vulkan handle of the underlying buffer for the current frame
     */
    VkBuffer getCurrentFrameRawBuffer() { return getCurrentFrameBuffer().getBuffer(); }

    /**
     * @brief Marks the given range dirty. Derived classes may use dirty ranges to inform copies
     *
     * @param i The index of the element to mark dirty
     * @param n The number of elements to mark dirty
     */
    void markDirty(std::uint32_t i, std::uint32_t n = 1) {
        dirtyRanges[currentDirtyRange].markDirty(i, n);
        queueTransfer();
    }

    /**
     * @brief Marks the entire buffer as dirty
     */
    void markFullDirty() { dirtyRanges.fill(DirtyRange(0, getSize())); }

    /**
     * @brief Sets whether to always copy the entire buffer on transfer. Default is false. When
     *        false a dirty range is used instead
     *
     * @param copyFull True to copy the entire buffer, false to only copy the changed sections
     */
    void setCopyFullRange(bool copyFull) { copyFullRange = copyFull; }

protected:
    /**
     * @brief Returns the renderer Vulkan state
     */
    vk::VulkanState& getVulkanState() { return *vulkanState; }

    /**
     * @brief Called when the buffer should be created
     *
     * @param vulkanState The renderer Vulkan state
     * @param numElements The number of data elements to size the buffer for
     */
    virtual void doCreate(vk::VulkanState& vulkanState, std::uint32_t numElements) = 0;

    /**
     * @brief Logs a warning. Derived classes should override this if used
     *
     * @param commandBuffer The command buffer to record into
     * @param context The transfer context
     */
    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& context) override {
        (void)commandBuffer;
        (void)context;
        BL_LOG_WARN << "executeTransfer called on buffer that does not implement it";
    }

    /**
     * @brief Returns the range of elements marked dirty in the current frame
     */
    DirtyRange getCurrentDirtyRange() const {
        return copyFullRange ? DirtyRange{0, getSize()} : dirtyRanges[currentDirtyRange];
    }

    /**
     * @brief Returns the total range of elements marked dirty for all frames in flight
     */
    DirtyRange getAccumulatedDirtyRange() const {
        if (copyFullRange) { return {0, getSize()}; }

        DirtyRange accum;
        for (const auto& r : dirtyRanges) { accum.combine(r); }
        return accum;
    }

    /**
     * @brief Call after performing a transfer of a dirty range
     */
    void markClean() {
        currentDirtyRange              = (currentDirtyRange + 1) % dirtyRanges.size();
        dirtyRanges[currentDirtyRange] = DirtyRange();
    }

private:
    std::uint32_t alignment;
    std::array<DirtyRange, cfg::Limits::MaxConcurrentFrames> dirtyRanges;
    std::uint32_t currentDirtyRange;
    bool copyFullRange;
};

} // namespace base
} // namespace buf
} // namespace rc
} // namespace bl

#endif
