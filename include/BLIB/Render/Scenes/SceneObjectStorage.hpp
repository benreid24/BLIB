#ifndef BLIB_RENDER_SCENES_SCENEOBJECTSTORAGE_HPP
#define BLIB_RENDER_SCENES_SCENEOBJECTSTORAGE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <queue>
#include <type_traits>
#include <vector>

namespace bl
{
namespace gfx
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
     * @brief Returns the ECS id for the given object key
     *
     * @param key The key to lookup
     * @return The ECS id for the given key
     */
    ecs::Entity getObjectEntity(Key key) const;

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

private:
    struct Bucket {
        std::vector<T> objects;
        std::queue<std::uint32_t> freeIds;
        std::vector<ecs::Entity> entityMap;

        Bucket() {
            objects.reserve(Scene::DefaultObjectCapacity);
            entityMap.resize(Scene::DefaultObjectCapacity, ecs::InvalidEntity);
        }
    };

    Bucket staticBucket;
    Bucket dynamicBucket;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
SceneObjectStorage<T>::AllocateResult SceneObjectStorage<T>::allocate(UpdateSpeed updateFreq,
                                                                      ecs::Entity entity) {
    AllocateResult result{};

    Bucket& bucket             = updateFreq == UpdateSpeed::Static ? staticBucket : dynamicBucket;
    result.originalBaseAddress = bucket.objects.data();
    if (!bucket.freeIds.empty()) {
        const std::uint32_t id = bucket.freeIds.front();
        bucket.freeIds.pop();

        result.newObject                   = &bucket.objects[id];
        result.newObject->sceneKey.sceneId = id;
        bucket.entityMap[id]               = entity;
    }
    else {
        const std::uint32_t id  = bucket.objects.size();
        result.addressesChanged = bucket.objects.size() == bucket.objects.capacity();
        bucket.entityMap.emplace_back(entity);
        result.newObject                   = &bucket.objects.emplace_back();
        result.newObject->sceneKey.sceneId = id;
    }

    result.newObject->sceneKey.updateFreq = updateFreq;
    result.newCapacity                    = bucket.objects.capacity();

    return result;
}

template<typename T>
ecs::Entity SceneObjectStorage<T>::getObjectEntity(Key key) const {
    const Bucket& bucket = key.updateFreq == UpdateSpeed::Static ? staticBucket : dynamicBucket;
    return bucket.entityMap[key.sceneId];
}

template<typename T>
inline T& SceneObjectStorage<T>::getObject(Key key) {
    const Bucket& bucket = key.updateFreq == UpdateSpeed::Static ? staticBucket : dynamicBucket;
    return bucket.objects[key.sceneId];
}

template<typename T>
void SceneObjectStorage<T>::release(Key key) {
    Bucket& bucket = key.updateFreq == UpdateSpeed::Static ? staticBucket : dynamicBucket;
    bucket.freeIds.emplace(key.sceneId);
    bucket.entityMap[key.sceneId] = ecs::InvalidEntity;
}

template<typename T>
void SceneObjectStorage<T>::unlinkAll(ds::DescriptorSetInstanceCache& descriptors) {
    UpdateSpeed speed = UpdateSpeed::Static;
    for (Bucket& bucket : {staticBucket, dynamicBucket}) {
        for (unsigned int i = 0; i < bucket.objects.size(); ++i) {
            if (bucket.entityMap[i] != ecs::InvalidEntity) {
                descriptors.unlinkSceneObject(bucket.entityMap[i], {speed, i});
            }
        }
        speed = UpdateSpeed::Dynamic;
    }
}

} // namespace scene
} // namespace gfx
} // namespace bl

#endif
