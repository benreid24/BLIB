#ifndef BLIB_RENDER_RESOURCES_SCENEREF_HPP
#define BLIB_RENDER_RESOURCES_SCENEREF_HPP

namespace bl
{
namespace rc
{
class Scene;

namespace res
{
class ScenePool;
}

/**
 * @brief Handle to a scene allocated from ScenePool. Performs reference counting
 *
 * @ingroup Renderer
 */
class SceneRef {
public:
    /**
     * @brief Creates an empty handle
     */
    SceneRef()
    : scene(nullptr)
    , handle(nullptr) {}

    /**
     * @brief Copies from the given ref
     *
     * @param copy The ref to copy from
     */
    SceneRef(const SceneRef& copy);

    /**
     * @brief Assumes ownership from the given handle
     *
     * @param move The handle to copy and invalidate
     */
    SceneRef(SceneRef&& move)
    : scene(move.scene)
    , handle(move.handle) {
        move.scene = nullptr;
    }

    /**
     * @brief Copies from the given ref
     *
     * @param copy The ref to copy from
     * @return A reference to this object
     */
    SceneRef& operator=(const SceneRef& copy);

    /**
     * @brief Assumes ownership from the given handle
     *
     * @param move The handle to copy and invalidate
     * @return A reference to this object
     */
    SceneRef& operator=(SceneRef&& move) {
        decrementRefCount();
        scene      = move.scene;
        handle     = move.handle;
        move.scene = nullptr;
        return *this;
    }

    /**
     * @brief Destroys the handle. If this was the last handle then the scene is also destroyed
     */
    ~SceneRef() { decrementRefCount(); }

    /**
     * @brief Returns whether or not the handle refers to a scene
     */
    bool isValid() const { return scene != nullptr; }

    /**
     * @brief Returns the underlying scene
     */
    Scene* operator->() { return scene; }

    /**
     * @brief Returns the underlying scene
     */
    const Scene* operator->() const { return scene; }

    /**
     * @brief Returns the underlying scene
     */
    Scene& operator*() { return *scene; }

    /**
     * @brief Returns the underlying scene
     */
    const Scene& operator*() const { return *scene; }

    /**
     * @brief Returns the underlying scene
     */
    Scene* get() { return scene; }

    /**
     * @brief Returns the underlying scene
     */
    const Scene* get() const { return scene; }

    /**
     * @brief Returns the underlying scene
     */
    operator Scene*() { return scene; }

    /**
     * @brief Returns the underlying scene
     */
    operator const Scene*() const { return scene; }

    /**
     * @brief Releases the handle to the scene
     */
    void release();

private:
    Scene* scene;
    void* handle;

    SceneRef(void* handle);
    void incrementRefCount();
    void decrementRefCount();

    friend class res::ScenePool;
};

} // namespace rc
} // namespace bl

#endif
