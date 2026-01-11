#ifndef BLIB_RENDER_RESOURCES_MATERIALREF_HPP
#define BLIB_RENDER_RESOURCES_MATERIALREF_HPP

#include <BLIB/Render/Materials/Material.hpp>
#include <cstdint>
#include <utility>

namespace bl
{
namespace rc
{
namespace res
{
class MaterialPool;

/**
 * @brief Reference to a material in the material pool
 *
 * @ingroup Renderer
 */
class MaterialRef {
public:
    /**
     * @brief Creates an invalid ref
     */
    MaterialRef()
    : owner(nullptr) {}

    /**
     * @brief Initializes this ref from another
     *
     * @tparam TOther The derived type of the other ref
     * @param other The ref to copy
     */
    MaterialRef(const MaterialRef& other)
    : MaterialRef() {
        *this = other;
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     */
    MaterialRef(MaterialRef&& other)
    : MaterialRef() {
        *this = std::forward<MaterialRef>(other);
    }

    /**
     * @brief Releases the ref and possibly the resource if the ref count is zero
     */
    ~MaterialRef() { release(); }

    /**
     * @brief Initializes this ref from another
     *
     * @param other The ref to copy
     * @return A reference to this object
     */
    MaterialRef& operator=(const MaterialRef& other) {
        decrement();
        owner = other.owner;
        id    = other.id;
        if (owner) { increment(); }
        return *this;
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     * @return A reference to this object
     */
    MaterialRef& operator=(MaterialRef&& other) {
        decrement();
        owner       = other.owner;
        other.owner = nullptr;
        id          = 0;
        return *this;
    }

    /**
     * @brief Access the underlying resource value
     */
    const mat::Material& operator*() const;

    /**
     * @brief Access the underlying resource value
     */
    const mat::Material* operator->() const;

    /**
     * @brief Returns a modifiable reference to the material and marks it for update
     */
    mat::Material& getForEdit();

    /**
     * @brief Returns the underlying material id
     */
    std::uint32_t getId() const { return id; }

    /**
     * @brief Returns whether this ref points to a resource or not
     */
    bool isValid() const { return owner != nullptr; }

    /**
     * @brief Returns whether this ref points to a resource or not
     */
    operator bool() const { return owner != nullptr; }

    /**
     * @brief Releases this handle on the underlying resource
     */
    void release() {
        if (owner) {
            decrement();
            owner = nullptr;
            id    = 0;
        }
    }

private:
    MaterialPool* owner;
    std::uint32_t id;

    MaterialRef(MaterialPool* owner, std::uint32_t id);
    void increment();
    void decrement();

    friend class MaterialPool;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
