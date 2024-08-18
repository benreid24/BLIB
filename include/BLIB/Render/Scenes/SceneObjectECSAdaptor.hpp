#ifndef BLIB_RENDER_SCENES_SCENEOBJECTECSADAPTOR_HPP
#define BLIB_RENDER_SCENES_SCENEOBJECTECSADAPTOR_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Descriptors/DescriptorComponentStorage.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
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
 * @brief Similar to SceneObjectStorage, but uses the ECS as underlying storage
 *
 * @tparam T The type of SceneObject to provide storage for
 * @ingroup Renderer
 */
template<typename T>
class SceneObjectECSAdaptor {
    static_assert(std::is_same_v<T, SceneObject> || std::is_base_of_v<SceneObject, T>,
                  "T must be SceneObject or derived from SceneObject");

public:
    /**
     * @brief Reserves some memory
     */
    SceneObjectECSAdaptor(ecs::Registry& registry);

    /**
     * @brief Destroys all created components
     */
    ~SceneObjectECSAdaptor();

    /**
     * @brief Allocates a new scene object
     *
     * @param updateFreq The update speed of the new object
     * @param entity The ECS id of the new object
     * @return The new scene object
     */
    T* allocate(UpdateSpeed updateFreq, ecs::Entity entity);

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

    /**
     * @brief Returns a usable callback to map scene key to ECS id
     */
    ds::DescriptorComponentStorageBase::EntityCallback makeEntityCallback() const;

private:
    struct Mapping {
        util::IdAllocatorUnbounded<std::uint32_t> indexAlloc;
        std::vector<ecs::Entity> entityMap;

        Mapping()
        : indexAlloc(32) {
            entityMap.reserve(32);
        }

        std::uint32_t alloc(ecs::Entity ent) {
            const std::uint32_t i = indexAlloc.allocate();
            if (i >= entityMap.size()) { entityMap.resize(i + 1, ecs::InvalidEntity); }
            entityMap[i] = ent;
            return i;
        }

        void release(std::uint32_t i) {
            if (i < entityMap.size()) {
                indexAlloc.release(i);
                entityMap[i] = ecs::InvalidEntity;
            }
        }
    };

    ecs::Registry& registry;
    Mapping mapStatic;
    Mapping mapDynamic;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
SceneObjectECSAdaptor<T>::SceneObjectECSAdaptor(ecs::Registry& registry)
: registry(registry) {}

template<typename T>
SceneObjectECSAdaptor<T>::~SceneObjectECSAdaptor() {
    const auto release = [this](Mapping& m) {
        for (unsigned int i = 0; i < m.entityMap.size(); ++i) {
            if (m.indexAlloc.isAllocated(i)) { registry.removeComponent<T>(m.entityMap[i]); }
        }
    };

    release(mapStatic);
    release(mapDynamic);
}

template<typename T>
T* SceneObjectECSAdaptor<T>::allocate(UpdateSpeed updateFreq, ecs::Entity entity) {
    Mapping& bucket       = updateFreq == UpdateSpeed::Static ? mapStatic : mapDynamic;
    const std::uint32_t i = bucket.alloc(entity);
    T* object             = registry.emplaceComponent<T>(entity);
    object->sceneKey      = Key{updateFreq, i};
    return object;
}

template<typename T>
ecs::Entity SceneObjectECSAdaptor<T>::getObjectEntity(Key key) const {
    const Mapping& bucket = key.updateFreq == UpdateSpeed::Static ? mapStatic : mapDynamic;
    return key.sceneId < bucket.entityMap.size() ? bucket.entityMap[key.sceneId] :
                                                   ecs::InvalidEntity;
}

template<typename T>
inline T& SceneObjectECSAdaptor<T>::getObject(Key key) {
    return *registry.getComponent<T>(getObjectEntity(key));
}

template<typename T>
void SceneObjectECSAdaptor<T>::release(Key key) {
    Mapping& bucket = key.updateFreq == UpdateSpeed::Static ? mapStatic : mapDynamic;
    bucket.release(key.sceneId);
}

template<typename T>
void SceneObjectECSAdaptor<T>::unlinkAll(ds::DescriptorSetInstanceCache& descriptors) {
    UpdateSpeed speed = UpdateSpeed::Static;
    for (Mapping* bucket : {&mapStatic, &mapDynamic}) {
        for (unsigned int i = 0; i < bucket->indexAlloc.endId(); ++i) {
            if (bucket->entityMap[i] != ecs::InvalidEntity) {
                descriptors.unlinkSceneObject(bucket->entityMap[i], {speed, i});
            }
        }
        speed = UpdateSpeed::Dynamic;
    }
}

template<typename T>
ds::DescriptorComponentStorageBase::EntityCallback SceneObjectECSAdaptor<T>::makeEntityCallback()
    const {
    return [this](scene::Key key) { return getObjectEntity(key); };
}

} // namespace scene
} // namespace rc
} // namespace bl

#endif
