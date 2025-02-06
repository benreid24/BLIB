#ifndef BLIB_RENDER_MATERIALS_MATERIALID_HPP
#define BLIB_RENDER_MATERIALS_MATERIALID_HPP

#include <cstdint>

namespace bl
{
namespace rc
{
namespace mat
{
/**
 * @brief Helper struct to disambiguate material and texture ids for descriptor bindings
 *
 * @ingroup Renderer
 */
struct MaterialId {
    std::uint32_t id;

    /**
     * @brief Creates a 0 id
     */
    MaterialId()
    : id(0) {}

    /**
     * @brief Creates and assigns the id
     *
     * @param id The material id
     */
    MaterialId(std::uint32_t id)
    : id(id) {}

    /**
     * @brief Overwrites the id
     *
     * @param nid The new id to overwrite
     * @return A reference to this object
     */
    MaterialId& operator=(std::uint32_t nid) {
        id = nid;
        return *this;
    }

    /**
     * @brief Converts the object to the underlying id type
     */
    operator std::uint32_t() const { return id; }
};

} // namespace mat
} // namespace rc
} // namespace bl

#endif
