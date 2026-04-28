#ifndef BLIB_ASSETS_DETAIL_DEPENDENCYLISTBASE_HPP
#define BLIB_ASSETS_DETAIL_DEPENDENCYLISTBASE_HPP

#include <BLIB/Assets/DependencyPolicy.hpp>
#include <BLIB/Assets/Detail/DependencyChain.hpp>
#include <BLIB/Assets/LoadPolicy.hpp>
#include <BLIB/Assets/Ref.hpp>
#include <BLIB/Assets/State.hpp>
#include <vector>

namespace bl
{
namespace as
{
class Payload;

namespace detail
{
/**
 * @brief Implementation detail. Intermediate base class for a list of dependencies
 *
 * @ingroup Assets
 */
class DependencyListBase : public DependencyChain {
public:
    /**
     * @brief Creates the dependency chain node
     *
     * @param repo The asset repository
     * @param owner The payload that owns this dependency
     * @param tag The tag of this specific dependency
     */
    DependencyListBase(Repository& repo, Payload& owner, std::string_view tag, LoadPolicy policy,
                       DependencyPolicy depPolicy);

    /**
     * @brief Releases the ref to the dependency if it is loaded and has no other refs
     */
    virtual ~DependencyListBase() = default;

    /**
     * @brief Adds a new dependency to the list. Should only be called during asset creation
     *
     * @param uuid The UUID of the asset to add as a dependency
     * @return True if the dependency was successfully initialized and loaded, false otherwise
     */
    bool addDependency(util::UUID uuid);

    /**
     * @brief Ensures that the dependency is valid and loaded
     *
     * @return True if the dependency is valid and loaded, false otherwise
     */
    virtual bool load() override;

    /**
     * @brief Release the ref to the depenency which may result in it being unloaded
     */
    virtual void unload() override;

    /**
     * @brief Returns the number of dependencies in the list
     */
    std::size_t getSize() const { return dependencies.size(); }

    /**
     * @brief Returns the dependency at the given index
     *
     * @param index The index of the dependency to return
     * @return A ref to the given dependency
     */
    Ref getItem(std::size_t index) const;

protected:
    struct Entry {
        util::UUID uuid;
        Ref dependency;
    };

    const LoadPolicy policy;
    const DependencyPolicy depPolicy;
    std::vector<Entry> dependencies;

    /**
     * @brief Performs post deserialization initialization
     */
    virtual bool matchAndLoad(const std::vector<RepoDependency>& dependencies) override;

private:
    bool loadEntry(Entry& entry);
    bool getStatus(bool allSuccess, bool anySuccess) const;

    friend class Payload;
};

} // namespace detail
} // namespace as
} // namespace bl

#endif
