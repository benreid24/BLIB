#ifndef BLIB_ASSETS_DEPENDENCYLIST_HPP
#define BLIB_ASSETS_DEPENDENCYLIST_HPP

#include <BLIB/Assets/Detail/DependencyListBase.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <string_view>
#include <type_traits>

namespace bl
{
namespace as
{
class Asset;

/**
 * @brief Dependency of a payload on a list of other assets
 *
 * @tparam T The type of payload of the dependency
 * @ingroup Assets
 */
template<typename T = Payload>
class DependencyList : public detail::DependencyListBase {
public:
    static_assert(std::is_base_of<Payload, T>::value, "T must be a Payload type");

    /**
     * @brief Creates the dependency list in the empty state
     *
     * @param repo The asset repository
     * @param owner The asset that owns this dependency
     * @param tag The asset local tag to identify this dependency
     */
    DependencyList(Repository& repo, Payload& owner, std::string_view tag)
    : DependencyListBase(repo, owner, tag) {}

    /**
     * @brief Destroys the dependency
     */
    ~DependencyList() = default;

    /**
     * @brief Returns the payload of the dependency at the given index
     */
    T& get(std::size_t i) { return getItem(i).getAsset().getPayload().template as<T>(); }

    /**
     * @brief Returns the payload of the dependency at the given index
     */
    const T& get(std::size_t i) const {
        return getItem(i).getAsset().getPayload().template as<T>();
    }

    /**
     * @brief Returns the asset of the dependency at the given index
     */
    Asset& getAsset() { return getItem(0).getAsset(); }

    /**
     * @brief Returns the asset of the dependency at the given index
     */
    const Asset& getAsset() const { return getItem(0).getAsset(); }
};

} // namespace as
} // namespace bl

#endif
