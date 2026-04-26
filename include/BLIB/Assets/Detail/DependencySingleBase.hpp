#ifndef BLIB_ASSETS_DETAIL_DEPENDENCYSINGLEBASE_HPP
#define BLIB_ASSETS_DETAIL_DEPENDENCYSINGLEBASE_HPP

#include <BLIB/Assets/Detail/DependencyChain.hpp>
#include <BLIB/Assets/Ref.hpp>
#include <BLIB/Assets/State.hpp>

namespace bl
{
namespace as
{
class Payload;

namespace detail
{
/**
 * @brief Implementation detail. Intermediate base class for individual dependencies
 *
 * @ingroup Assets
 */
class DependencySingleBase : private DependencyChain {
public:
    /**
     * @brief Creates the dependency chain node
     *
     * @param repo The asset repository
     * @param owner The payload that owns this dependency
     * @param tag The tag of this specific dependency
     */
    DependencySingleBase(Repository& repo, Payload& owner, std::string_view tag);

    /**
     * @brief Releases the ref to the dependency if it is loaded and has no other refs
     */
    virtual ~DependencySingleBase() = default;

    /**
     * @brief Returns the state of the dependency
     */
    State getState() const;

    /**
     * @brief Returns the UUID of the dependency
     */
    util::UUID getUUID() const;

    /**
     * @brief Initializes the dependency. Should only be called during asset creation
     *
     * @param uuid The UUID of the asset this dependency points to
     * @return True if the dependency was successfully initialized and loaded, false otherwise
     */
    bool init(util::UUID uuid);

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
     * @brief Returns whether the dependency points to a valid asset
     */
    bool isValid() const { return dependency.isValid(); }

protected:
    util::UUID uuid;
    Ref dependency;

    /**
     * @brief Performs post deserialization initialization
     */
    virtual bool matchAndLoad(const std::vector<RepoDependency>& dependencies) override;

    friend class Payload;
};

} // namespace detail
} // namespace as
} // namespace bl

#endif
