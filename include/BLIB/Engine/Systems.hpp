#ifndef BLIB_ENGINE_SYSTEMS_HPP
#define BLIB_ENGINE_SYSTEMS_HPP

#include <BLIB/Engine/FrameStage.hpp>
#include <BLIB/Engine/StateMask.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Logging.hpp>
#include <array>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;

/**
 * @brief Central registry for systems in the engine. Systems should be registered here before the
 *        engine is started otherwise their init() method will not be called
 *
 * @ingroup Engine
 */
class Systems {
    struct StageSet;

public:
    /// Signature of one-off tasks that can be registered to run
    using Task = std::function<void()>;

    /**
     * @brief Handle to a task submitted to Systems
     *
     * @ingroup Engine
     */
    class TaskHandle {
    public:
        /**
         * @brief Constructs a null handle
         */
        TaskHandle();

        /**
         * @brief Copy constructor. Invalidates the future in the copied-from task handle
         *
         * @param handle The handle to copy from
         */
        TaskHandle(const TaskHandle& handle);

        /**
         * @brief Move constructor. Invalidates the future in the copied-from task handle
         *
         * @param handle The handle to copy from
         */
        TaskHandle(TaskHandle&& handle) = default;

        /**
         * @brief Copy assignment. Invalidates the future in the copied-from task handle
         *
         * @param handle The handle to copy from
         * @return A reference to this handle
         */
        TaskHandle& operator=(const TaskHandle& handle);

        /**
         * @brief Move assignment. Invalidates the future in the copied-from task handle
         *
         * @param handle The handle to copy from
         * @return A reference to this handle
         */
        TaskHandle& operator=(TaskHandle&& handle) = default;

        /**
         * @brief Returns whether or not the handle refers to a valid task
         */
        bool isValid() const;

        /**
         * @brief Returns whether or not the task is currently queued and waiting to be executed
         */
        bool isQueued() const;

        /**
         * @brief Cancels the task if it has not yet ran. Safe to call on invalid handles
         */
        void cancel();

        /**
         * @brief Waits for the submitted task to complete. Safe to call on invalid handles
         */
        void wait();

    private:
        StageSet* owner;
        std::size_t index;
        std::uint16_t version;
        std::future<void> future;

        TaskHandle(StageSet* owner, std::size_t index, std::future<void>&& future);

        friend class Systems;
    };

    /**
     * @brief Creates and adds a new system to the registry
     *
     * @tparam T The type of system to add
     * @tparam ...TArgs Constructor argument types for the system
     * @param stage The frame stage to run the system in. Systems in the same stage may be
     *              ran simultaneously. Dependencies should run in later stages
     * @param stateMask Mask to use to selectively run the system during certain engine states
     * @param ...args Arguments to the system's constructor
     */
    template<typename T, typename... TArgs>
    T& registerSystem(FrameStage::V stage, StateMask::V stateMask, TArgs&&... args);

    /**
     * @brief Returns a reference to the given system. System must exist
     *
     * @tparam T The type of system to fetch
     * @return A reference to the system with the given type
     */
    template<typename T>
    T& getSystem();

    /**
     * @brief Returns a pointer to the given system if it exists
     *
     * @tparam T The type of system to fetch
     * @return A pointer to the system with the given type or nullptr
     */
    template<typename T>
    T* getSystemMaybe();

    /**
     * @brief Adds a one-off task to be executed in the given frame stage along with the systems in
     *        that stage. Tasks should be thread safe. The task queue is drained once processed
     *
     * @param stage The frame stage to execute the task in
     * @param task The task to execute
     */
    TaskHandle addFrameTask(FrameStage::V stage, Task&& task);

private:
    struct SystemInstance {
        StateMask::V mask;
        std::unique_ptr<System> system;

        SystemInstance(StateMask::V mask, std::unique_ptr<System>&& sys)
        : mask(mask)
        , system(std::forward<std::unique_ptr<System>>(sys)) {}
    };

    struct TaskEntry {
        std::packaged_task<void()> task;
        bool cancelled;

        TaskEntry(Task&& task)
        : task(std::forward<Task>(task))
        , cancelled(false) {}

        TaskEntry(TaskEntry&&) = default;

        void execute() {
            if (!cancelled) task();
        }
    };

    struct StageSet {
        std::mutex mutex;
        std::mutex taskMutex;
        std::vector<SystemInstance> systems;
        std::vector<TaskEntry> tasks;
        std::uint16_t version;

        StageSet();
        void drainTasks();
    };

    Engine& engine;
    std::array<StageSet, FrameStage::COUNT> systems;
    std::unordered_map<std::type_index, System*> typeMap;
    bool inited;

    Systems(Engine& engine);
    void init();
    void notifyFrameStart();
    void update(FrameStage::V startStage, FrameStage::V endStage, StateMask::V stateMask, float dt,
                float realDt, float lag, float realLag);
    void earlyCleanup();
    void cleanup();

    friend class Engine;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename... TArgs>
T& Systems::registerSystem(FrameStage::V stage, StateMask::V stateMask, TArgs&&... args) {
#ifdef BLIB_DEBUG
    const auto it = typeMap.find(typeid(T));
    if (it != typeMap.end()) {
        BL_LOG_CRITICAL << "System duplicated: " << typeid(T).name();
        throw std::runtime_error("Systems may only appear once per type");
    }

    if (stage >= systems.size()) {
        BL_LOG_CRITICAL << "Out of range frame stage index: " << stage;
        throw std::runtime_error("Out of range engine frame stage index");
    }
#endif

    systems[stage].systems.emplace_back(
        stateMask, std::move(std::make_unique<T>(std::forward<TArgs>(args)...)));
    System* s          = systems[stage].systems.back().system.get();
    typeMap[typeid(T)] = s;
    if (inited) { s->init(engine); }
    return static_cast<T&>(*s);
}

template<typename T>
T& Systems::getSystem() {
    const auto it = typeMap.find(typeid(T));
#ifdef BLIB_DEBUG
    if (it == typeMap.end()) {
        BL_LOG_CRITICAL << "System not found: " << typeid(T).name();
        throw std::runtime_error("System not found");
    }
#endif

    return *static_cast<T*>(it->second);
}

template<typename T>
T* Systems::getSystemMaybe() {
    const auto it = typeMap.find(typeid(T));
    if (it == typeMap.end()) { return nullptr; }

    return static_cast<T*>(it->second);
}

} // namespace engine
} // namespace bl

#endif
