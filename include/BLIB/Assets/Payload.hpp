#ifndef BLIB_ASSETS_PAYLOAD_HPP
#define BLIB_ASSETS_PAYLOAD_HPP

#include <BLIB/Assets/Mode.hpp>
#include <BLIB/Assets/Ref.hpp>
#include <BLIB/Assets/State.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/UUID.hpp>
#include <stdexcept>
#include <type_traits>

namespace bl
{
namespace as
{
template<typename T>
class Driver;
template<typename T>
class Dependency;
class Payload;
class Asset;
class Repository;

namespace detail
{
/**
 * @brief Implement detail. Helps Payload manage its tagged dependency list without allocations
 *
 * @ingroup Assets
 */
class DependencyChain {
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
    ~DependencyChain() = default;

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
    bool load();

    /**
     * @brief Release the ref to the depenency which may result in it being unloaded
     */
    void unload();

protected:
    const std::string_view tag;
    Repository& repo;
    Payload& owner;
    util::UUID uuid;
    Ref dependency;
    DependencyChain* next;

    friend class Payload;
};
} // namespace detail

/**
 * @brief Base class for asset payloads. Payloads are the actual data of assets such as images
 *
 * @ingroup Assets
 */
class Payload {
public:
    /**
     * @brief Struct of parameters passed to Payload constructors. All derived classes should be
     *        constructible from this struct
     */
    struct ConstructContext {
        Mode mode;
        Repository& repo;
        Asset& asset;
    };

    /**
     * @brief Destroys the payload
     */
    virtual ~Payload() = default;

    /**
     * @brief Returns the repository this payload belongs to
     */
    Repository& getRepository() { return repo; }

    /**
     * @brief Returns the asset this payload belongs to
     */
    Asset& getAsset() { return owner; }

    /**
     * @brief Safely casts the payload to the given type. Will throw if the cast is invalid
     *
     * @tparam T The payload type to cast to
     * @return A reference to this object as the given type
     */
    template<typename T>
    T& as() {
        static_assert(std::is_base_of_v<Payload, T>, "T must be a subclass of Payload");
        T* casted = dynamic_cast<T*>(this);
        if (!casted) {
            BL_LOG_ERROR << "Invalid Asset payload cast from " << typeid(*this).name() << " to "
                         << typeid(T).name();
            throw std::bad_cast();
        }
        return *casted;
    }

    /**
     * @brief Tests whether the payload is of the given type
     *
     * @tparam T The type to test for
     * @return True if this payload is T, false otherwise
     */
    template<typename T>
    bool is() const {
        static_assert(std::is_base_of_v<Payload, T>, "T must be a subclass of Payload");
        return dynamic_cast<const T*>(this) != nullptr;
    }

protected:
    /**
     * @brief Creates the payload
     *
     * @param ctx The context to construct with
     */
    Payload(const ConstructContext& ctx);

private:
    Repository& repo;
    Asset& owner;
    detail::DependencyChain* dependencyChain;

    void registerDependency(detail::DependencyChain* chain);
    bool loadDependencies();

    template<typename T>
    friend class Driver;
    friend class detail::DependencyChain;
};

} // namespace as
} // namespace bl

#endif
