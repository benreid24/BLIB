#ifndef BLIB_RENDER_GRAPH_TASKOUTPUT_HPP
#define BLIB_RENDER_GRAPH_TASKOUTPUT_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <initializer_list>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace bl
{
namespace rc
{
namespace rg
{
/**
 * @brief Wrapper for created task outputs that has some extra info for sharing
 *
 * @ingroup Renderer
 */
struct TaskOutput {
    /// Represents what creates this task output
    enum CreateMode {
        /// The output is created by this task or another task
        CreatedByTask,

        /// The output may only be created by another task
        CreatedByOtherTask,

        /// The output is external to the render graph (ie swapframe)
        CreatedExternally
    };

    /// Represents whether tasks can share the same output asset
    enum ShareMode {
        /// This input may not be shared with other tasks
        Exclusive,

        /// This input may be shared with other tasks
        Shared
    };

    /**
     * @brief The position order that the task should be in when multiple tasks write to this
     *        output. Tasks with the same Order will be ordered arbitrarily. Integer values may also
     *        be used to order tasks.
     */
    enum Order {
        /// The task should be the first to write to this output
        First = 0,

        /// The task should not be first or last to write to the output
        Middle = 10,

        /// The task should be the last to write to this output
        Last = 20
    };

    /**
     * @brief Descriptor for an output option for this slot
     */
    struct OutputOption {
        std::string_view tag;
        ShareMode shareMode;
        CreateMode createMode;

        /**
         * @brief Does nothing
         */
        OutputOption() = default;

        /**
         * @brief Creates the option descriptor
         *
         * @param tag The tag for this option
         * @param shareMode The share mode for this option
         * @param createMode The create mode for this option
         */
        OutputOption(std::string_view tag, ShareMode shareMode, CreateMode createMode)
        : tag(tag)
        , shareMode(shareMode)
        , createMode(createMode) {}
    };

    ctr::StaticVector<OutputOption, 8> options;
    Order order;
    ctr::StaticVector<std::string_view, 4> sharedWith;
    std::string_view purpose;

    /**
     * @brief Creates an empty task output
     */
    TaskOutput()
    : order(Middle) {}

    /**
     * @brief Creates the task output
     *
     * @param tag The tag of the output
     * @param createMode The creation mode of the output
     * @param order The order the task should output in
     * @param sharedWith The tags of the possible tasks that this output must be shared with
     */
    TaskOutput(std::string_view tag, CreateMode createMode, ShareMode shareMode = Shared,
               Order order = Middle, std::initializer_list<std::string_view> sharedWith = {})
    : order(order) {
        options.emplace_back(tag, shareMode, createMode);
        for (const auto& tag : sharedWith) { this->sharedWith.emplace_back(tag); }
    }

    /**
     * @brief Creates the task output with multiple options. Options are prioritized in the order
     *        that they are given
     *
     * @param tags The different tags that may be outputted in this slot
     * @param createModes The creation modes of the output tags
     * @param order The order the task should output in
     * @param sharedWith The tags of the possible tasks that this output must be shared with
     */
    TaskOutput(std::initializer_list<std::string_view> tags,
               std::initializer_list<CreateMode> createModes,
               std::initializer_list<ShareMode> shareModes = {Shared}, Order order = Middle,
               std::initializer_list<std::string_view> sharedWith = {})
    : order(order) {
        if (tags.size() != createModes.size()) {
            throw std::runtime_error("Mismatched tag and create mode count");
        }
        if (shareModes.size() != 1 && shareModes.size() != tags.size()) {
            throw std::runtime_error("Mismatched share mode and tag count");
        }

        for (unsigned int i = 0; i < tags.size(); ++i) {
            const ShareMode shareMode =
                shareModes.size() > 1 ? *(shareModes.begin() + i) : *shareModes.begin();
            options.emplace_back(*(tags.begin() + i), shareMode, *(createModes.begin() + i));
        }

        for (const auto& tag : sharedWith) { this->sharedWith.emplace_back(tag); }
    }

    /**
     * @brief Adds an option for this output slot. Options are prioritized in the order they are
     *        added, including those passed in the constructor
     *
     * @param tag The tag of the output option
     * @param shareMode The share mode of the output option
     * @param createMode The create mode of the output option
     * @return A reference to this object
     */
    TaskOutput& addOption(std::string_view tag, ShareMode shareMode, CreateMode createMode) {
        options.emplace_back(tag, shareMode, createMode);
        return *this;
    }

    /**
     * @brief Sets the purpose of this output. purpose is used to disambiguate assets that have the
     *        same tags and is generally used by descriptor sets
     *
     * @param p The purpose of this output
     * @return A reference to this object
     */
    TaskOutput& withPurpose(std::string_view p) {
        purpose = p;
        return *this;
    }

    /**
     * @brief Sets the order priority of this output. Order is relative to the other outputs being
     *        generated in a given timeline slice
     *
     * @param o The order priority of this output
     * @return A reference to this object
     */
    TaskOutput& withOrder(Order o) {
        order = o;
        return *this;
    }

    /**
     * @brief Adds a task id that this output may be shared with
     *
     * @param task The id of the task that this output may be shared with
     * @return A reference to this object
     */
    TaskOutput& withSharedWith(std::string_view task) {
        sharedWith.emplace_back(task);
        return *this;
    }
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
