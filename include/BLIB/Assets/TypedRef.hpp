#ifndef BLIB_ASSETS_TYPEDREF_HPP
#define BLIB_ASSETS_TYPEDREF_HPP

#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Assets/Ref.hpp>
#include <type_traits>
#include <utility>

namespace bl
{
namespace as
{
/**
 * @brief Helper class to reference assets with known payload types
 *
 * @tparam T The type of payload of the asset this ref points to
 * @ingroup Assets
 */
template<typename T>
class TypedRef : public Ref {
    static_assert(std::is_base_of<Payload, T>::value, "T must be an Payload type");

public:
    /**
     * @brief Creates an empty ref
     */
    TypedRef() = default;

    /**
     * @brief Copies the ref
     *
     * @param ref The ref to copy from
     */
    TypedRef(const TypedRef& ref) = default;

    /**
     * @brief Adopts from the given ref
     *
     * @param ref The ref to adopt from
     */
    TypedRef(TypedRef&& ref) = default;

    /**
     * @brief Releases the ref
     */
    ~TypedRef() = default;

    /**
     * @brief Copies the ref
     *
     * @param ref The ref to copy
     * @return A reference to this object
     */
    TypedRef& operator=(const TypedRef& ref) = default;

    /**
     * @brief Adopts from the given ref
     *
     * @param ref The ref to adopt from
     * @return A reference to this object
     */
    TypedRef& operator=(TypedRef&& ref) = default;

    /**
     * @brief Constructs from an untyped ref and validates the type. The ref must point to a loaded
     *        asset with the correct payload type
     *
     * @param ref The ref to construct from
     */
    TypedRef(Ref&& ref)
    : Ref(std::forward<Ref>(ref)) {
        validateAfterBaseRefConstruct();
    }

    /**
     * @brief Constructs from an untyped ref and validates the type. The ref must point to a loaded
     *        asset with the correct payload type
     *
     * @param ref The ref to construct from
     */
    TypedRef(const Ref& ref)
    : Ref(ref) {
        validateAfterBaseRefConstruct();
    }

    /**
     * @brief Returns a reference to the typed payload
     */
    T& payload() { return getAsset().getPayload().as<T>(); }

    /**
     * @brief Returns a reference to the typed payload
     */
    const T& payload() const { return getAsset().getPayload().as<T>(); }

    /**
     * @brief Returns a reference to the typed payload
     */
    T& operator*() { return payload(); }

    /**
     * @brief Returns a reference to the typed payload
     */
    const T& operator*() const { return payload(); }

    /**
     * @brief Returns a pointer to the typed payload
     */
    T* operator->() { return &payload(); }

    /**
     * @brief Returns a pointer to the typed payload
     */
    const T* operator->() const { return &payload(); }

    /**
     * @brief Returns whether this ref is in a valid state and the payload can be accessed
     */
    bool isValid() const {
        if (!Ref::isValid()) { return false; }
        if (getState() != State::Loaded) { return false; }
        if (!getAsset().getPayload().is<T>()) { return false; }
        return true;
    }

    /**
     * @brief Returns whether this ref is in a valid state and the payload can be accessed
     */
    operator bool() const { return isValid(); }

private:
    void validateAfterBaseRefConstruct() {
        if (isValid()) {
            if (getState() != State::Loaded) {
                BL_LOG_WARN << "Constructed TypedRef for asset " << getAsset().getUUID().toString()
                            << " but it is not in state Loaded";
                return;
            }
            if (!getAsset().getPayload().is<T>()) {
                BL_LOG_ERROR << "Constructed TypedRef for asset " << getAsset().getUUID().toString()
                             << " but it does not have the correct payload type";
                release();
            }
        }
    }
};

} // namespace as
} // namespace bl

#endif
