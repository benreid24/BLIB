#ifndef BLIB_RENDER_BUFFERS_ALIGNEDBUFFER_HPP
#define BLIB_RENDER_BUFFERS_ALIGNEDBUFFER_HPP

#include <BLIB/Render/Buffers/Alignment.hpp>
#include <BLIB/Vulkan.hpp>
#include <stdexcept>
#include <vector>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Utility class to store a buffer of PODs on the CPU aligned for the UBO requirements of the
 *        device being used
 *
 * @tparam T The type of object to store. MUST be a POD
 * @ingroup Renderer
 */
template<typename T>
class AlignedBuffer {
public:
    /**
     * @brief Creates an empty buffer
     */
    AlignedBuffer();

    /**
     * @brief Copies from the existing buffer
     */
    AlignedBuffer(const AlignedBuffer&) = default;

    /**
     * @brief Copies from the existing buffer
     */
    AlignedBuffer(AlignedBuffer&&) = default;

    /**
     * @brief Creates a new buffer of the given size
     *
     * @param vulkanState Renderer vulkan state
     * @param usage The use-case to align for
     * @param size Number of elements in the buffer
     */
    AlignedBuffer(Alignment usage, std::uint32_t size);

    /**
     * @brief Creates a new buffer of the given size. Must be called before any operations may be
     *        performed
     *
     * @param vulkanState Renderer vulkan state
     * @param usage The use-case to align for
     * @param size Number of elements in the buffer
     */
    void create(Alignment usage, std::uint32_t size);

    /**
     * @brief Empties the CPU buffer
     */
    void clear();

    /**
     * @brief Returns the element at the given index
     *
     * @param i Index of the element to access. Not bounds checked
     * @return A reference to the element at the given index
     */
    T& operator[](std::uint32_t i);

    /**
     * @brief Returns the element at the given index
     *
     * @param i Index of the element to access. Not bounds checked
     * @return A reference to the element at the given index
     */
    const T& operator[](std::uint32_t i) const;

    /**
     * @brief Creates a new element at the end of the buffer
     *
     * @tparam ...TArgs Argument types to the element constructor
     * @param ...args Arguments to the element constructor
     */
    template<typename... TArgs>
    void emplace(TArgs&&... args);

    /**
     * @brief Adds a new element to the end of the buffer
     *
     * @param value The value to add
     */
    void append(const T& value);

    /**
     * @brief Erases the element at the given index. Copies the element from the end of the buffer
     *        to save time. Element destructors are never called
     *
     * @param i Index of the element to erase
     */
    void erase(std::uint32_t i);

    /**
     * @brief Resizes the buffer to hold the given number of elements. Prior values are preserved
     *
     * @param size The number of elements to store
     */
    void resize(std::uint32_t size);

    /**
     * @brief Fills the buffer with the given value
     *
     * @param value The value to copy into all existing slots
     */
    void fill(const T& value);

    /**
     * @brief Returns the number of elements in the buffer
     */
    constexpr std::uint32_t size() const;

    /**
     * @brief Returns the memory address to use for copies
     */
    constexpr const void* data() const;

    /**
     * @brief Returns the number of bytes to copy when transferring to the GPU
     */
    constexpr std::uint32_t alignedSize() const;

    /**
     * @brief Returns the space used by each element in the aligned storage
     */
    constexpr std::uint32_t elementSize() const;

private:
    std::vector<char> storage;
    std::uint32_t alignment;
    std::uint32_t storedElements;

    T* cast(std::size_t i);
    const T* cast(std::size_t i) const;
    void ensureCapacity(std::size_t size);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
AlignedBuffer<T>::AlignedBuffer()
: alignment(0)
, storedElements(0) {}

template<typename T>
AlignedBuffer<T>::AlignedBuffer(Alignment use, std::uint32_t size) {
    create(use, size);
}

template<typename T>
void AlignedBuffer<T>::create(Alignment use, std::uint32_t size) {
    alignment = computeAlignment(sizeof(T), use);
    resize(size);
}

template<typename T>
T& AlignedBuffer<T>::operator[](std::uint32_t i) {
#ifdef BLIB_DEBUG
    if (alignment == 0) {
        throw std::runtime_error("AlignedBufer must be initialized from device properties");
    }
#endif
    return *cast(i);
}

template<typename T>
const T& AlignedBuffer<T>::operator[](std::uint32_t i) const {
#ifdef BLIB_DEBUG
    if (alignment == 0) {
        throw std::runtime_error("AlignedBufer must be initialized from device properties");
    }
#endif
    return *cast(i);
}

template<typename T>
void AlignedBuffer<T>::append(const T& value) {
#ifdef BLIB_DEBUG
    if (alignment == 0) {
        throw std::runtime_error("AlignedBufer must be initialized from device properties");
    }
#endif
    ensureCapacity(storedElements + 1);
    new (cast(storedElements)) T(value);
    ++storedElements;
}

template<typename T>
void AlignedBuffer<T>::erase(std::uint32_t i) {
    if (storedElements > 1 && i < storedElements - 1) { *cast(i) = *cast(storedElements - 1); }
    --storedElements;
}

template<typename T>
void AlignedBuffer<T>::resize(std::uint32_t size) {
#ifdef BLIB_DEBUG
    if (alignment == 0) {
        throw std::runtime_error("AlignedBufer must be initialized from device properties");
    }
#endif

    ensureCapacity(size);
    storedElements = std::max(storedElements, size);
}

template<typename T>
void AlignedBuffer<T>::clear() {
    storage.clear();
    storedElements = 0;
}

template<typename T>
void AlignedBuffer<T>::fill(const T& value) {
    for (std::uint32_t j = 0; j < storedElements; ++j) { new (cast(j)) T(value); }
}

template<typename T>
constexpr std::uint32_t AlignedBuffer<T>::size() const {
    return storedElements;
}

template<typename T>
inline constexpr const void* AlignedBuffer<T>::data() const {
    return static_cast<const void*>(storage.data());
}

template<typename T>
inline constexpr std::uint32_t AlignedBuffer<T>::alignedSize() const {
    return storedElements * alignment;
}

template<typename T>
constexpr std::uint32_t AlignedBuffer<T>::elementSize() const {
    return alignment;
}

template<typename T>
T* AlignedBuffer<T>::cast(std::size_t i) {
    return static_cast<T*>(static_cast<void*>(storage.data() + alignment * i));
}

template<typename T>
const T* AlignedBuffer<T>::cast(std::size_t i) const {
    return static_cast<const T*>(static_cast<const void*>(storage.data() + alignment * i));
}

template<typename T>
void AlignedBuffer<T>::ensureCapacity(std::size_t size) {
    const auto bytes = size * alignment;
    if (bytes > storage.size()) { storage.resize(bytes); }
}

template<typename T>
template<typename... TArgs>
void AlignedBuffer<T>::emplace(TArgs&&... args) {
#ifdef BLIB_DEBUG
    if (alignment == 0) {
        throw std::runtime_error("AlignedBufer must be initialized from device properties");
    }
#endif
    ensureCapacity(storedElements + 1);
    new (cast(storedElements)) T(std::forward<TArgs>(args)...);
    ++storedElements;
}

} // namespace buf
} // namespace rc
} // namespace bl

#endif
