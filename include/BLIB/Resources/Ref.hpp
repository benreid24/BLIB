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
     *
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
     * @brief Destroy the Ref
     *
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
     * @brief Copies this Ref from another one and invalidates the original
     *
     * @param copy The Ref to copy and invalidate
     * @return Ref& A reference to this object
     */
    Ref& operator=(Ref&& copy);

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
     *
     */
    void forceInCache(bool force = true);

    /**
     * @brief Releases the held resource
     *
     */
    void release();

private:
    Resource<TResourceType>* resource;

    Ref(Resource<TResourceType>* resource);

    friend class ResourceManager<TResourceType>;
    friend struct gui::Font;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Ref<T>::Ref()
: resource(nullptr) {}

template<typename T>
Ref<T>::Ref(Resource<T>* r)
: resource(r) {
    ++resource->refCount;
}

template<typename T>
Ref<T>::Ref(const Ref& copy)
: resource(copy.resource) {
    ++resource->refCount;
}

template<typename T>
Ref<T>::Ref(Ref&& copy)
: resource(copy.resource) {
    copy.resource = nullptr;
}

template<typename T>
Ref<T>::~Ref() {
    release();
}

template<typename T>
Ref<T>& Ref<T>::operator=(const Ref& copy) {
    release();
    resource = copy.resource;
    ++resource->refCount;
    return *this;
}

template<typename T>
Ref<T>& Ref<T>::operator=(Ref&& copy) {
    release();
    resource      = copy.resource;
    copy.resource = nullptr;
    return *this;
}

template<typename T>
constexpr T& Ref<T>::operator*() {
    return resource->data;
}

template<typename T>
constexpr const T& Ref<T>::operator*() const {
    return resource->data;
}

template<typename T>
constexpr T* Ref<T>::operator->() {
    return &resource->data;
}

template<typename T>
constexpr const T* Ref<T>::operator->() const {
    return &resource->data;
}

template<typename T>
constexpr T* Ref<T>::get() {
    return &resource->data;
}

template<typename T>
constexpr const T* Ref<T>::get() const {
    return &resource->data;
}

template<typename T>
Ref<T>::operator bool() const {
    return resource != nullptr;
}

template<typename T>
void Ref<T>::forceInCache(bool f) {
    resource->forceInCache = f;
}

template<typename T>
void Ref<T>::release() {
    if (resource) {
        --resource->refCount;
        resource = nullptr;
    }
}

} // namespace resource
} // namespace bl

#endif
