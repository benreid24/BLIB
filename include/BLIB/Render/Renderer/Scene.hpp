#ifndef BLIB_RENDER_RENDERER_SCENE_HPP
#define BLIB_RENDER_RENDERER_SCENE_HPP

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Render/Renderer/Object.hpp>
#include <array>
#include <glad/vulkan.h>
#include <mutex>
#include <vector>

namespace bl
{
namespace render
{
class Scene {
public:
    Scene();

    ~Scene();

    /**
     * @brief Creates a new object to be rendered in the scene
     *
     * @param owner The Renderable which will own the Object
     * @return Object* The newly created object
     */
    Object::Handle createAndAddObject(Renderable* owner);

    /**
     * @brief Removes the given object from the scene
     *
     * @param object The object to remove
     */
    void removeObject(const Object::Handle& object);

private:
    std::mutex mutex;
    container::ObjectPool<Object> objects;
};

} // namespace render
} // namespace bl

#endif
