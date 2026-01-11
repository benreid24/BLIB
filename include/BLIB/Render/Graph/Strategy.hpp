#ifndef BLIB_RENDER_GRAPH_STRATEGY_HPP
#define BLIB_RENDER_GRAPH_STRATEGY_HPP

namespace bl
{
namespace rc
{
namespace rg
{
class RenderGraph;

/**
 * @brief Base class for renderer strategies. Render strategies populate render graphs with tasks
 *        when they are first created
 *
 * @ingroup Renderer
 */
class Strategy {
public:
    /**
     * @brief Destroys the strategy
     */
    virtual ~Strategy() = default;

    /**
     * @brief Populates the given graph with tasks
     *
     * @param graph The graph to populate
     */
    virtual void populate(RenderGraph& graph) = 0;

    /**
     * @brief Returns the version of the strategy. This is used to rebuild graphs on settings change
     */
    unsigned int getVersion() const { return version; }

protected:
    /**
     * @brief Creates the strategy
     */
    Strategy()
    : version(0) {}

    /**
     * @brief Causes all created graphs to be rebuilt lazily
     */
    void invalidateGraphs() { ++version; }

private:
    unsigned int version;
};

} // namespace rg
} // namespace rc
} // namespace bl

#endif
