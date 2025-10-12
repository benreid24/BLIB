#ifndef BLIB_RENDER_SCENES_TARGETTABLE_HPP
#define BLIB_RENDER_SCENES_TARGETTABLE_HPP

#include <vector>

namespace bl
{
namespace rc
{
class RenderTarget;

namespace scene
{
/**
 * @brief Helper class to manage a list of render targets and provide indices for them
 *
 * @ingroup Renderer
 */
class TargetTable {
public:
    /**
     * @brief Creates an empty table
     */
    TargetTable() = default;

    /**
     * @brief Adds a render target to the table. Does not check if already present
     *
     * @param target The target to add
     * @return The index of the added target
     */
    unsigned int addTarget(RenderTarget* target);

    /**
     * @brief Returns the index of the target in the table, or max uint if not present
     *
     * @param target The target to find the index of
     * @return The index of the target, or max uint if not present
     */
    unsigned int getTargetIndex(RenderTarget* target) const;

    /**
     * @brief Returns the render target at the given index
     *
     * @param index The index to fetch, must be valid
     * @return Pointer to the render target at the specified index
     */
    RenderTarget* getTarget(unsigned int index) const;

    /**
     * @brief Returns the next index id which is also the size of the table
     */
    unsigned int nextId() const;

private:
    std::vector<RenderTarget*> targets;
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
