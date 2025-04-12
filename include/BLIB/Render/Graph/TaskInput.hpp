#ifndef BLIB_RENDER_GRAPH_TASKINPUT_HPP
#define BLIB_RENDER_GRAPH_TASKINPUT_HPP

#include <initializer_list>
#include <string_view>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
/**
 * @brief Wrapper for a single input for a task. Allows multiple tags to be options for a single
 *        input slot
 *
 * @ingroup Renderer
 */
struct TaskInput {
    /// Represents how inputs are shared by different tasks
    enum ShareMode {
        /// This input may not be shared with other tasks
        Exclusive,

        /// This input may be shared with other tasks
        Shared
    };

    std::vector<std::string_view> options;
    ShareMode shareMode;

    /**
     * @brief Creates an empty input
     */
    TaskInput()
    : shareMode(Exclusive) {}

    /**
     * @brief Creates an input with a single asset tag option
     *
     * @param input The tag of the asset for this input slot
     * @param shareMode The sharing behavior of the input
     */
    TaskInput(std::string_view input, ShareMode shareMode = Shared)
    : options(1, input)
    , shareMode(shareMode) {}

    /**
     * @brief Creates an input with a collection of possible tags for the input slot
     *
     * @param inputs The different options for the input slot
     * @param shareMode The sharing behavior of the input
     */
    TaskInput(std::initializer_list<std::string_view> inputs, ShareMode shareMode = Shared)
    : options(inputs)
    , shareMode(shareMode) {}
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
