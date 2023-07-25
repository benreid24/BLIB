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
    std::vector<std::string_view> options;

    /**
     * @brief Creates an empty input
     */
    TaskInput() = default;

    /**
     * @brief Creates an input with a single asset tag option
     *
     * @param input The tag of the asset for this input slot
     */
    TaskInput(std::string_view input)
    : options(1, input) {}

    /**
     * @brief Creates an input with a collection of possible tags for the input slot
     *
     * @param inputs The different options for the input slot
     */
    TaskInput(std::initializer_list<std::string_view> inputs)
    : options(inputs) {}
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
