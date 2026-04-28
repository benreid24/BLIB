#ifndef BLIB_ASSETS_DETAIL_DEPENDENCYCHAIN_HPP
#define BLIB_ASSETS_DETAIL_DEPENDENCYCHAIN_HPP

#include <BLIB/Assets/RepoDependency.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/UUID.hpp>
#include <string_view>

namespace bl
{
namespace as
{
class Payload;
class Repository;

namespace detail
{
/**
 * @brief Implementation detail. Helps Payload manage its tagged dependency list without allocations
 *
 * @ingroup Assets
 */
class DependencyChain : private util::NonCopyable {
public:
    /**
     * @brief Creates the dependency chain node
     *
     * @param repo The asset repository
     * @param owner The payload that owns this dependency
     * @param tag The tag of this specific dependency
     */
    DependencyChain(Repository& repo, Payload& owner, std::string_view tag);

    /**
     * @brief Releases the ref to the dependency if it is loaded and has no other refs
     */
    virtual ~DependencyChain() = default;

    /**
     * @brief Ensures that the dependency is valid and loaded
     *
     * @return True if the dependency is valid and loaded, false otherwise
     */
    virtual bool load() = 0;

    /**
     * @brief Release the ref to the depenency which may result in it being unloaded
     */
    virtual void unload() = 0;

protected:
    const std::string_view tag;
    Repository& repo;
    Payload& owner;
    DependencyChain* next;

    /**
     * @brief Registers the given dependency with the repository
     *
     * @param uuid The UUID of the dependency to register
     */
    void registerDependency(util::UUID uuid);

    /**
     * @brief Unregisters the given dependency with the repository
     *
     * @param uuid The UUID of the dependency to unregister
     * @return True if the dependency was successfully unregistered, false otherwise
     */
    bool unregisterDependency(util::UUID uuid);

    /**
     * @brief Performs post deserialization initialization and loads the dependency
     */
    virtual bool matchAndLoad(const std::vector<RepoDependency>& dependencies) = 0;

    friend class Payload;
};

} // namespace detail
} // namespace as
} // namespace bl

#endif
