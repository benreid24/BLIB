#ifndef BLIB_RENDER_SCENES_SCENEOBJECTSTORAGE_HPP
#define BLIB_RENDER_SCENES_SCENEOBJECTSTORAGE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Config/Constants.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/ShaderResources/EntityComponentShaderResource.hpp>
#include <BLIB/Util/IdAllocatorUnbounded.hpp>
#include <queue>
#include <type_traits>
#include <vector>

namespace bl
{
namespace rc
{
namespace scene
{
/**
 * @brief Storage for scene objects. Splits objects by update frequency
 *
 * @tparam T The type of SceneObject to provide storage for
 * @ingroup Renderer
 */
template<typename T>
class SceneObjectStorage {
    static_assert(std::is_same_v<T, SceneObject> || std::is_base_of_v<SceneObject, T>,
                  "T must be SceneObject or derived from SceneObject");

public:
    /**
     * @brief Represents the result of an allocation of an object
     */
    struct AllocateResult {
        T* newObject;
        bool addressesChanged;
        T* originalBaseAddress;
        std::uint32_t newCapacity;
    };

    /**
     * @brief Reserves some memory
     */
    SceneObjectStorage() = default;

    /**
     * @brief Allocates a new scene object, growing the buffer if required
     *
     * @param updateFreq The update speed of the new object
     * @param entity The ECS id of the new object
     * @return The result of the allocation
     */
    AllocateResult allocate(UpdateSpeed updateFreq, ecs::Entity entity);

    /**
     * @brief Fetches the object with the given key
     *
     * @param key The key of the object to fetch
     * @return A reference to the given object
     */
    T& getObject(Key key);

    /**
     * @brief Marks the given key as being free. Allows future allocation to reuse it
     *
     * @param key The key to release
     */
    void release(Key key);

    /**
     * @brief Unlinks created objects
     *
     * @param descriptors The descriptor sets to unbind
     */
    void unlinkAll(ds::DescriptorSetInstanceCache& descriptors);

    /**
     * @brief Invokes the provided callback for each element in the collection
     *
     * @tparam TCallback The type of the callback function or callable object
     * @param callback A callable object to be invoked for each element
     */
    template<typename TCallback>
    void forEach(TCallback&& callback);

    /**
     * @brief Gets a new pointer from an old pointer. Call after a pool grows
     *
     * @param speed The object pool to rebase on
     * @param original The original pointer
     * @param oldBase The original pool base pointer
     * @return The new object pointer
     */
    T* rebase(UpdateSpeed speed, T* original, T* oldBase);

private:
    struct Bucket {
        std::vector<T> objects;
        util::IdAllocatorUnbounded<std::uint32_t> idAllocator;

        Bucket() { objects.reserve(cfg::Constants::DefaultSceneObjectCapacity); }
    };

    Bucket staticBucket;
    Bucket dynamicBucket;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
typename SceneObjectStorage<T>::AllocateResult SceneObjectStorage<T>::allocate(
    UpdateSpeed updateFreq, ecs::Entity entity) {
    AllocateResult result{};

    Bucket& bucket             = updateFreq == UpdateSpeed::Static ? staticBucket : dynamicBucket;
    result.originalBaseAddress = bucket.objects.data();

    const std::uint32_t id = bucket.idAllocator.allocate();
    if (id >= bucket.objects.size()) { bucket.objects.resize(id + 1); }

    result.addressesChanged               = bucket.objects.data() != result.originalBaseAddress;
    result.newObject                      = &bucket.objects[id];
    result.newObject->sceneKey.sceneId    = id;
    result.newObject->entity              = entity;
    result.newObject->sceneKey.updateFreq = updateFreq;
    result.newCapacity                    = bucket.objects.capacity();

    return result;
}

template<typename T>
inline T& SceneObjectStorage<T>::getObject(Key key) {
    Bucket& bucket = key.updateFreq == UpdateSpeed::Static ? staticBucket : dynamicBucket;
    return bucket.objects[key.sceneId];
}

template<typename T>
void SceneObjectStorage<T>::release(Key key) {
    Bucket& bucket = key.updateFreq == UpdateSpeed::Static ? staticBucket : dynamicBucket;
    bucket.idAllocator.release(key.sceneId);
}

template<typename T>
void SceneObjectStorage<T>::unlinkAll(ds::DescriptorSetInstanceCache& descriptors) {
    UpdateSpeed speed = UpdateSpeed::Static;
    for (Bucket* bucket : {&staticBucket, &dynamicBucket}) {
        for (unsigned int i = 0; i < bucket->objects.size(); ++i) {
            if (bucket->idAllocator.isAllocated(i)) {
                descriptors.unlinkSceneObject(bucket->objects[i].entity, {speed, i});
            }
        }
        speed = UpdateSpeed::Dynamic;
    }
}

template<typename T>
T* SceneObjectStorage<T>::rebase(UpdateSpeed speed, T* og, T* ob) {
    auto& bucket = speed == UpdateSpeed::Static ? staticBucket : dynamicBucket;
    return bucket.objects.data() + (og - ob);
}

template<typename T>
template<typename TCallback>
void SceneObjectStorage<T>::forEach(TCallback&& callback) {
    UpdateSpeed speed = UpdateSpeed::Static;
    for (Bucket* bucket : {&staticBucket, &dynamicBucket}) {
        for (unsigned int i = 0; i < bucket->objects.size(); ++i) {
            if (bucket->idAllocator.isAllocated(i)) { callback(bucket->objects[i]); }
        }
        speed = UpdateSpeed::Dynamic;
    }
}

} // namespace scene
} // namespace rc
} // namespace bl

#endif
