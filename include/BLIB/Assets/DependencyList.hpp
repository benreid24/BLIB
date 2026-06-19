#ifndef BLIB_ASSETS_DEPENDENCYLIST_HPP
#define BLIB_ASSETS_DEPENDENCYLIST_HPP

#include <BLIB/Assets/Detail/DependencyListBase.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Assets/TypedRef.hpp>
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
 * @tparam Policy The load policy for this dependency
 * @tparam DepPolicy The dependency policy for this dependency
 * @ingroup Assets
 */
template<typename T = Payload, LoadPolicy Policy = LoadPolicy::Eager,
         DependencyPolicy DepPolicy = DependencyPolicy::Required>
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
    : DependencyListBase(repo, owner, tag, Policy, DepPolicy) {}

    /**
     * @brief Destroys the dependency
     */
    ~DependencyList() = default;

    /**
     * @brief Returns the payload of the dependency at the given index
     *
     * @param i The index of the dependency to get
     */
    T& get(std::size_t i) {
        ensure(i);
        return getItem(i).getAsset().getPayload().template as<T>();
    }

    /**
     * @brief Returns the payload of the dependency at the given index
     *
     * @param i The index of the dependency to get
     */
    const T& get(std::size_t i) const {
        ensure(i);
        return getItem(i).getAsset().getPayload().template as<T>();
    }

    /**
     * @brief Returns the payload of the dependency at the given index
     *
     * @param i The index of the dependency to get
     * @return A ref to the payload of the dependency at the given index
     */
    TypedRef<T> getRef(std::size_t i) const {
        ensure(i);
        return TypedRef<T>(getItem(i));
    }

    /**
     * @brief Returns the asset of the dependency at the given index
     *
     * @param i The index of the dependency to get
     */
    Asset& getAsset(unsigned int i) { return getItem(i).getAsset(); }

    /**
     * @brief Returns the asset of the dependency at the given index
     *
     * @param i The index of the dependency to get
     */
    const Asset& getAsset(unsigned int i) const { return getItem(i).getAsset(); }

private:
    void ensure(unsigned int i) const {
        if (i >= dependencies.size()) {
            BL_LOG_ERROR << "Dependency list " << tag << " index " << i << " out of range";
            return;
        }

        auto ref = getItem(i);
        if (ref->getState() != State::Loaded) {
            if constexpr (Policy == LoadPolicy::Manual) {
                BL_LOG_ERROR
                    << "Attempted to access dependency that is not loaded with Manual load policy: "
                    << ref.getAsset().getUUID().toString() << " (" << tag << "-" << i
                    << "). Attempting auto-load";
            }
            if (!ref->load()) {
                BL_LOG_ERROR << "Failed to load dependency: " << ref.getAsset().getUUID().toString()
                             << " (" << tag << "-" << i << ")";
            }
        }
    }
};

} // namespace as
} // namespace bl

#endif
