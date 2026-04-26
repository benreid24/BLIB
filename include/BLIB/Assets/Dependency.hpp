#ifndef BLIB_ASSETS_DEPENDENCY_HPP
#define BLIB_ASSETS_DEPENDENCY_HPP

#include <BLIB/Assets/Detail/DependencySingleBase.hpp>
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
 * @ingroup Assets
 */
template<typename T = Payload>
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
    : DependencySingleBase(repo, owner, tag) {}

    /**
     * @brief Destroys the dependency
     */
    ~Dependency() = default;

    /**
     * @brief Returns the payload of the dependency
     */
    T& get() { return dependency.getAsset().getPayload().template as<T>(); }

    /**
     * @brief Returns the payload of the dependency
     */
    const T& get() const { return dependency.getAsset().getPayload().template as<T>(); }

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
};

} // namespace as
} // namespace bl

#endif
