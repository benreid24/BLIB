#ifndef BLIB_RESOURCES_REF_HPP
#define BLIB_RESOURCES_REF_HPP

#include <BLIB/Resources/Resource.hpp>

namespace bl
{
namespace gui
{
struct Font;
}
namespace resource
{
template<typename T>
class ResourceManager;

/**
 * @brief Represents a reference to a managed resource. Performs reference counting, but resources
 *        are never deallocated by this, only by ResourceManager
 *
 * @tparam TResourceType The resource type being referenced
 * @ingroup Resources
 */
template<typename TResourceType>
class Ref {
public:
    /**
     * @brief Construct an empty Ref that does not refer to any resource
     */
    Ref();

    /**
     * @brief Construct a new Ref from a Ref to be copied
     *
     * @param copy The Ref to copy
     */
    Ref(const Ref& copy);

    /**
     * @brief Construct a new Ref from a Ref to be copied
     *
     * @param copy The Ref to copy. Is invalidated by this call
     */
    Ref(Ref&& copy);

    /**
     * @brief Helper to cast between compatible pointer types
     *
     * @tparam TOther A compatible type to cast this ref to
     * @param copy The Ref to copy
     */
    template<typename TOther>
    Ref(const Ref<TOther>& copy);

    /**
     * @brief Helper to cast between compatible pointer types
     *
     * @tparam TOther A compatible type to cast this ref to
     * @param copy The Ref to copy. Is invalidated by this call
     */
    template<typename TOther>
    Ref(Ref<TOther>&& move);

    /**
     * @brief Destroy the Ref
     */
    ~Ref();

    /**
     * @brief Copies this Ref from another one
     *
     * @param copy The Ref to copy
     * @return Ref& A reference to this object
     */
    Ref& operator=(const Ref& copy);

    /**
     * @brief Helper to cast between compatible pointer types
     *
     * @tparam TOther A compatible type to cast this ref to
     * @param copy The Ref to copy
     * @return Ref& A reference to this object
     */
    template<typename TOther>
    Ref& operator=(const Ref<TOther>& copy);

    /**
     * @brief Copies this Ref from another one and invalidates the original
     *
     * @param copy The Ref to copy and invalidate
     * @return Ref& A reference to this object
     */
    Ref& operator=(Ref&& copy) noexcept;

    /**
     * @brief Helper to cast between compatible pointer types
     *
     * @tparam TOther A compatible type to cast this ref to
     * @param copy The Ref to copy and invalidate
     * @return Ref& A reference to this object
     */
    template<typename TOther>
    Ref& operator=(Ref<TOther>&& move) noexcept;

    /**
     * @brief Dereference the ref into the underlying resource
     *
     * @return TResourceType& A reference to the underlying resource
     */
    constexpr TResourceType& operator*();

    /**
     * @brief Dereference the ref into the underlying resource
     *
     * @return TResourceType& A reference to the underlying resource
     */
    constexpr const TResourceType& operator*() const;

    /**
     * @brief Dereference the ref into the underlying resource
     *
     * @return TResourceType& A pointer to the underlying resource
     */
    constexpr TResourceType* operator->();

    /**
     * @brief Dereference the ref into the underlying resource
     *
     * @return TResourceType& A pointer to the underlying resource
     */
    constexpr const TResourceType* operator->() const;

    /**
     * @brief Directly get the pointer to the underlying resource
     *
     * @return TResourceType* A pointer to the underlying resource
     */
    constexpr TResourceType* get();

    /**
     * @brief Directly get the pointer to the underlying resource
     *
     * @return TResourceType* A pointer to the underlying resource
     */
    constexpr const TResourceType* get() const;

    /**
     * @brief Returns whether or not the Ref refers to a resource
     *
     * @return True if a resource is pointed to, false otherwise
     */
    operator bool() const;

    /**
     * @brief Calling this will keep the underlying resource in memory even if all references are
     *        released
     *
     * @param force True to keep in memory always, false to allow the resource to be freed
     */
    void forceInCache(bool force = true);

    /**
     * @brief Releases the held resource
     */
    void release();

private:
    TResourceType* data;
    unsigned int* refCount;
    bool* forceCache;

    Ref(Resource<TResourceType>* resource);

    template<typename TOther>
    friend class Ref;
    friend class ResourceManager<TResourceType>;
    friend struct gui::Font;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Ref<T>::Ref()
: data(nullptr)
, refCount(nullptr)
, forceCache(nullptr) {}

template<typename T>
Ref<T>::Ref(Resource<T>* r)
: data(&r->data)
, refCount(&r->refCount)
, forceCache(&r->forceInCache) {
    ++(*refCount);
}

template<typename T>
Ref<T>::Ref(const Ref& copy)
: data(copy.data)
, refCount(copy.refCount)
, forceCache(copy.forceCache) {
    if (refCount) { ++(*refCount); }
}

template<typename T>
template<typename U>
Ref<T>::Ref(const Ref<U>& copy)
: data(copy.data)
, refCount(copy.refCount)
, forceCache(copy.forceCache) {
    static_assert(std::is_base_of_v<U, T>, "Ref may only be converted to Ref of base");
    if (refCount) { ++(*refCount); }
}

template<typename T>
Ref<T>::Ref(Ref&& copy)
: data(copy.data)
, refCount(copy.refCount)
, forceCache(copy.forceCache) {
    copy.data       = nullptr;
    copy.refCount   = nullptr;
    copy.forceCache = nullptr;
}

template<typename T>
template<typename U>
Ref<T>::Ref(Ref<U>&& copy)
: data(copy.data)
, refCount(copy.refCount)
, forceCache(copy.forceCache) {
    static_assert(std::is_base_of_v<U, T>, "Ref may only be converted to Ref of base");
    copy.data       = nullptr;
    copy.refCount   = nullptr;
    copy.forceCache = nullptr;
}

template<typename T>
Ref<T>::~Ref() {
    release();
}

template<typename T>
Ref<T>& Ref<T>::operator=(const Ref& copy) {
    release();
    data       = copy.data;
    refCount   = copy.refCount;
    forceCache = copy.forceCache;
    if (refCount) { ++(*refCount); }
    return *this;
}

template<typename T>
template<typename U>
Ref<T>& Ref<T>::operator=(const Ref<U>& copy) {
    static_assert(std::is_base_of_v<U, T>, "Ref may only be converted to Ref of base");

    release();
    data       = copy.data;
    refCount   = copy.refCount;
    forceCache = copy.forceCache;
    if (refCount) { ++(*refCount); }
    return *this;
}

template<typename T>
Ref<T>& Ref<T>::operator=(Ref&& copy) noexcept {
    release();
    data            = copy.data;
    refCount        = copy.refCount;
    forceCache      = copy.forceCache;
    copy.data       = nullptr;
    copy.refCount   = nullptr;
    copy.forceCache = nullptr;
    return *this;
}

template<typename T>
template<typename U>
Ref<T>& Ref<T>::operator=(Ref<U>&& copy) noexcept {
    static_assert(std::is_base_of_v<U, T>, "Ref may only be converted to Ref of base");

    release();
    data            = copy.data;
    refCount        = copy.refCount;
    forceCache      = copy.forceCache;
    copy.data       = nullptr;
    copy.refCount   = nullptr;
    copy.forceCache = nullptr;
    return *this;
}

template<typename T>
constexpr T& Ref<T>::operator*() {
    return *data;
}

template<typename T>
constexpr const T& Ref<T>::operator*() const {
    return *data;
}

template<typename T>
constexpr T* Ref<T>::operator->() {
    return data;
}

template<typename T>
constexpr const T* Ref<T>::operator->() const {
    return data;
}

template<typename T>
constexpr T* Ref<T>::get() {
    return data;
}

template<typename T>
constexpr const T* Ref<T>::get() const {
    return data;
}

template<typename T>
Ref<T>::operator bool() const {
    return data != nullptr;
}

template<typename T>
void Ref<T>::forceInCache(bool f) {
    *forceInCache = f;
}

template<typename T>
void Ref<T>::release() {
    if (refCount) {
        --(*refCount);
        data       = nullptr;
        refCount   = nullptr;
        forceCache = nullptr;
    }
}

} // namespace resource
} // namespace bl

#endif
