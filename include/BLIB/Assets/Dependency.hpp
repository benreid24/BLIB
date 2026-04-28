#ifndef BLIB_ASSETS_DEPENDENCY_HPP
#define BLIB_ASSETS_DEPENDENCY_HPP

#include <BLIB/Assets/DependencyPolicy.hpp>
#include <BLIB/Assets/Detail/DependencySingleBase.hpp>
#include <BLIB/Assets/LoadPolicy.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <string_view>
#include <type_traits>

namespace bl
{
namespace as
{
/**
 * @brief Dependency of a payload on another asset
 *
 * @tparam T The type of payload of the dependency
 * @tparam Policy The load policy for this dependency
 * @tparam DepPolicy The dependency policy for this dependency
 * @ingroup Assets
 */
template<typename T = Payload, LoadPolicy Policy = LoadPolicy::Eager,
         DependencyPolicy DepPolicy = DependencyPolicy::Required>
class Dependency : public detail::DependencySingleBase {
public:
    static_assert(std::is_base_of<Payload, T>::value, "T must be a Payload type");

    /**
     * @brief Creates the dependency in the empty state
     *
     * @param repo The asset repository
     * @param owner The asset that owns this dependency
     * @param tag The asset local tag to identify this dependency
     */
    Dependency(Repository& repo, Payload& owner, std::string_view tag)
    : DependencySingleBase(repo, owner, tag, Policy, DepPolicy) {}

    /**
     * @brief Destroys the dependency
     */
    ~Dependency() = default;

    /**
     * @brief Returns the payload of the dependency
     */
    T& get() {
        ensure();
        return dependency.getAsset().getPayload().template as<T>();
    }

    /**
     * @brief Returns the payload of the dependency
     */
    const T& get() const {
        const_cast<Dependency*>(this)->ensure();
        return dependency.getAsset().getPayload().template as<T>();
    }

    /**
     * @brief Returns the payload of the dependency
     */
    T* operator->() { return &get(); }

    /**
     * @brief Returns the payload of the dependency
     */
    const T* operator->() const { return &get(); }

    /**
     * @brief Returns the payload of the dependency
     */
    T& operator*() { return get(); }

    /**
     * @brief Returns the payload of the dependency
     */
    const T& operator*() const { return get(); }

    /**
     * @brief Returns the asset of the dependency
     */
    Asset& getAsset() { return dependency.getAsset(); }

    /**
     * @brief Returns the asset of the dependency
     */
    const Asset& getAsset() const { return dependency.getAsset(); }

private:
    void ensure() {
        if (uuid == util::UUID()) {
            BL_LOG_ERROR << "Accessing invalid dependency: " << uuid.toString() << " (" << tag
                         << ")";
            return;
        }
        if (getState() != State::Loaded) {
            if constexpr (Policy == LoadPolicy::Manual) {
                BL_LOG_ERROR
                    << "Attempted to access dependency that is not loaded with Manual load policy: "
                    << uuid.toString() << " (" << tag << "). Attempting auto-load";
            }
            if (!load()) {
                BL_LOG_ERROR << "Failed to load dependency: " << uuid.toString() << " (" << tag
                             << ")";
            }
        }
    }
};

} // namespace as
} // namespace bl

#endif
