#ifndef BLIB_RENDER_SCENES_KEY_HPP
#define BLIB_RENDER_SCENES_KEY_HPP

#include <BLIB/Render/UpdateSpeed.hpp>
#include <BLIB/Util/HashCombine.hpp>
#include <cstdint>
#include <functional>
#include <limits>

namespace bl
{
namespace gfx
{
namespace scene
{
/**
 * @brief Key for objects in scenes. Contains update speed and scene id. Scene ids are not unique
 *        across update speeds
 *
 * @ingroup Renderer
 */
struct Key {
    UpdateSpeed updateFreq;
    std::uint32_t sceneId;

    /**
     * @brief Creates an invalid key
     */
    Key()
    : updateFreq(UpdateSpeed::Dynamic)
    , sceneId(std::numeric_limits<std::uint32_t>::max()) {}

    /**
     * @brief Creates the key from the given components
     *
     * @param updateFreq The update speed of the scene object
     * @param sceneId The id of the object in the scene
     */
    Key(UpdateSpeed updateFreq, std::uint32_t sceneId)
    : updateFreq(updateFreq)
    , sceneId(sceneId) {}

    /**
     * @brief Tests for equality with the given key
     *
     * @param right The key to compare with
     * @return True if the keys are the same, false otherwise
     */
    constexpr bool operator==(const Key& right) const {
        return sceneId == right.sceneId && updateFreq == right.updateFreq;
    }

    /**
     * @brief Tests for inequality with the given key
     *
     * @param right The key to compare with
     * @return True if the keys are different, false otherwise
     */
    constexpr bool operator!=(const Key& right) const {
        return sceneId != right.sceneId || updateFreq != right.updateFreq;
    }

    /**
     * @brief Comparator for sorting keys
     *
     * @param right The key to compare with
     * @return True if this key is less than the right key, false otherwise
     */
    constexpr bool operator<(const Key& right) const {
        return updateFreq < right.updateFreq || sceneId < right.sceneId;
    }
};

} // namespace scene
} // namespace gfx
} // namespace bl

namespace std
{
template<>
struct hash<bl::gfx::scene::Key> {
    std::size_t operator()(const bl::gfx::scene::Key& key) const {
        return bl::util::hashCombine(hash<uint32_t>()(key.sceneId),
                                     hash<bl::gfx::UpdateSpeed>()(key.updateFreq));
    }
};
} // namespace std

#endif
