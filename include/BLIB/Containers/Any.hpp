#ifndef BLIB_CONTAINERS_ANY_HPP
#define BLIB_CONTAINERS_ANY_HPP

#include <BLIB/Logging.hpp>
#include <cstdint>
#include <cstdlib>

namespace bl
{
/**
 * @brief Fixed size equivalent of std::any. Specified size is for in place storage. Stored
 * objects smaller than that size will be stored in place, which allows sequential storage for
 * cache locality optimization. Larger objects will require allocations. In place storage size
 * is always used, even for types requiring allocation. Care must be taken to balance trying to
 * fit many types inplace while minimizing waste for unused inplace storage
 *
 * @tparam Size Number of bytes to use for in place storage
 * @ingroup Util
 *
 */
template<unsigned int Size>
class Any {
public:
    /**
     * @brief Constructs an empty object
     *
     */
    Any();

    /**
     * @brief Copy constructs with the given value
     *
     * @tparam T Type of object to become
     * @param value Value to store
     */
    template<typename T>
    Any(const T& value);

    /**
     * @brief Cleans up and destroys any contained object
     *
     */
    ~Any();

    /**
     * @brief Copies from another Any object and inherits type and value
     *
     * @param copy The object to copy
     */
    Any(const Any<Size>& copy);

    /**
     * @brief Takes ownership of the data in copy
     *
     * @param copy The object to copy
     */
    Any(Any&& copy);

    /**
     * @brief Copies from another Any object and inherits type and value
     *
     * @param copy The object to copy
     */
    Any& operator=(const Any<Size>& copy);

    /**
     * @brief Constructs a new value in place
     *
     * @tparam T Type of object to construct
     * @tparam TArgs Types of arguments for the constructor
     * @param args Arguments for constructor
     */
    template<typename T, typename... TArgs>
    void emplace(TArgs... args);

    /**
     * @brief Copy assigns a new type and value into the Any. Copies are optimized if the type
     * is unchanged
     *
     * @tparam T Type of value to become
     * @param value The value to store
     * @return Any& A reference to this object
     */
    template<typename T>
    Any& operator=(const T& value);

    /**
     * @brief Attempts to return the contained value as the given type. The program is aborted
     * if the type is incorrect
     *
     * @tparam T Type to retrieve
     * @return T& Reference to the contained value
     */
    template<typename T>
    T& get();

    /**
     * @brief Attempts to return the contained value as the given type. The program is aborted
     * if the type is incorrect
     *
     * @tparam T Type to retrieve
     * @return T& Reference to the contained value
     */
    template<typename T>
    const T& get() const;

    /**
     * @brief Returns whether or not the Any contains a value
     *
     */
    bool hasValue() const;

    /**
     * @brief Clears any contained value
     *
     */
    void clear();

private:
    typedef void (*OpFunc)(Any<Size>&, const Any<Size>*);

    template<typename T>
    static void operate(Any<Size>& obj, const Any<Size>* copy);

    std::uint8_t inplace[Size];
    void* object;
    bool heap;
    OpFunc ctype;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<unsigned int Size>
Any<Size>::Any()
: heap(false)
, ctype(nullptr)
, object(nullptr) {}

template<unsigned int Size>
template<typename T>
Any<Size>::Any(const T& value)
: Any() {
    *this = value;
}

template<unsigned int Size>
Any<Size>::~Any() {
    clear();
}

template<unsigned int Size>
Any<Size>::Any(const Any<Size>& copy)
: heap(copy.heap)
, ctype(copy.ctype) {
    ctype(*this, &copy);
}

template<unsigned int Size>
Any<Size>::Any(Any<Size>&& copy)
: object(copy.object)
, heap(copy.heap)
, ctype(copy.ctype) {
    if (!heap) {
        std::memcpy(inplace, copy.inplace, Size);
        object = inplace;
    }
    copy.heap = false; // ensure copy does not free this memory
}

template<unsigned int Size>
Any<Size>& Any<Size>::operator=(const Any<Size>& c) {
    heap  = c.heap;
    ctype = c.ctype;
    ctype(*this, &c);
    return *this;
}

template<unsigned int Size>
template<typename T>
Any<Size>& Any<Size>::operator=(const T& value) {
    if (ctype == &Any<Size>::operate<T>) {
        *static_cast<T*>(object) = value;
        return *this;
    }

    clear();
    if constexpr (sizeof(T) < Size) {
        object                   = inplace;
        *static_cast<T*>(object) = value;
        heap                     = false;
    }
    else {
        object = new T(value);
        heap   = true;
    }
    ctype = &Any<Size>::operate<T>;
    return *this;
}

template<unsigned int Size>
template<typename T, typename... TArgs>
void Any<Size>::emplace(TArgs... args) {
    clear();
    if constexpr (sizeof(T) <= Size) {
        object = inplace;
        heap   = false;
        object = new (object) T(args...);
    }
    else {
        object = new T(args...);
        heap   = true;
    }
    ctype = &Any<Size>::operate<T>;
}

template<unsigned int Size>
template<typename T>
T& Any<Size>::get() {
    if (ctype != &Any<Size>::operate<T>) {
        BL_LOG_ERROR << "Bad Any cast";
        abort();
    }
    return *static_cast<T*>(object);
}

template<unsigned int Size>
template<typename T>
const T& Any<Size>::get() const {
    if (ctype != &Any<Size>::operate<T>) {
        BL_LOG_ERROR << "Bad Any cast";
        abort();
    }
    return *static_cast<T*>(object);
}

template<unsigned int Size>
bool Any<Size>::hasValue() const {
    return ctype != nullptr;
}

template<unsigned int Size>
void Any<Size>::clear() {
    if (ctype) ctype(*this, nullptr);
    object = nullptr;
    heap   = false;
    ctype  = nullptr;
}

template<unsigned int Size>
template<typename T>
void Any<Size>::operate(Any<Size>& obj, const Any<Size>* copy) {
    if (copy) {
        if (obj.heap) { obj.object = new T(*static_cast<T*>(copy->object)); }
        else {
            obj.object                   = obj.inplace;
            *static_cast<T*>(obj.object) = *static_cast<T*>(copy->object);
        }
    }
    else {
        if (obj.heap) { delete static_cast<T*>(obj.object); }
        else if (obj.object) {
            static_cast<T*>(obj.object)->~T();
        }
    }
}

} // namespace bl

#endif
