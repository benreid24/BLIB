#ifndef BLIB_ENGINE_SYSTEMS_HPP
#define BLIB_ENGINE_SYSTEMS_HPP

#include <BLIB/Engine/FrameStage.hpp>
#include <BLIB/Engine/StateMask.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Logging.hpp>
#include <array>
#include <memory>
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
public:
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
    void registerSystem(FrameStage::V stage, StateMask::V stateMask, TArgs&&... args);

    /**
     * @brief Returns a reference to the given system. System must exist
     *
     * @tparam T The type of system to fetch
     * @return A reference to the system with the given type
     */
    template<typename T>
    T& getSystem();

private:
    struct SystemInstance {
        StateMask::V mask;
        std::unique_ptr<System> system;

        SystemInstance(StateMask::V mask, std::unique_ptr<System>&& sys)
        : mask(mask)
        , system(std::forward<std::unique_ptr<System>>(sys)) {}
    };

    struct StageSet {
        std::mutex mutex;
        std::vector<SystemInstance> systems;

        StageSet();
    };

    Engine& engine;
    std::array<StageSet, FrameStage::COUNT> systems;
    std::unordered_map<std::type_index, System*> typeMap;

    Systems(Engine& engine);
    void init();
    void update(FrameStage::V startStage, FrameStage::V endStage, StateMask::V stateMask, float dt);

    friend class Engine;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename... TArgs>
void Systems::registerSystem(FrameStage::V stage, StateMask::V stateMask, TArgs&&... args) {
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
    typeMap[typeid(T)] = systems[stage].systems.back().system.get();
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

} // namespace engine
} // namespace bl

#endif
