#ifndef BLIB_COMPONENTS_NAVMESH_HPP
#define BLIB_COMPONENTS_NAVMESH_HPP

#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <Recast.h>
#include <glm/glm.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Navmesh component for 3d terrain
 *
 * @ingroup Components
 */
class Navmesh {
public:
    /**
     * @brief Parameters used to build the navmesh
     */
    struct BuildParams {
        float cellSize   = 0.3f;
        float cellHeight = 0.2f;
        glm::vec3 topLeftBound;
        glm::vec3 bottomRightBound;
        float borderSize           = 0.f;
        float maxSlopeAngleDegrees = 45.f;
        float agentHeight;
        float agentRadius;
        float agentMaxClimb;
        float maxEdgeLength        = 12.f;
        float maxEdgeError         = 1.3f;
        float minIslandArea        = 1.f;
        float mergeIslandArea      = 3.f;
        float detailSampleDistance = 6.f;
        float detailSampleMaxError = 1.f;
    };

    /**
     * @brief The input terrain geometry
     */
    struct GeometryInput {
        glm::vec3* vertices;
        unsigned int vertexStride = sizeof(glm::vec3);
        unsigned int vertexCount;
        std::uint32_t* indices;
        unsigned int indexCount;
    };

    /**
     * @brief Flags that indicate which postprocessing to apply
     */
    struct PostprocessingFlags {
        bool filterLowHangingObstacles    = false;
        bool filterLedgeSpans             = false;
        bool filterWalkableLowHeightSpans = false;
    };

    /**
     * @brief Does nothing
     */
    Navmesh();

    /**
     * @brief Releases resources
     */
    ~Navmesh();

    /**
     * @brief Builds the navmesh
     *
     * @param params The parameters for the build
     * @param geometry The geometry to build the navmesh from
     * @param flags The postprocessing flags to apply
     * @return True if the navmesh was able to be built, false otherwise
     */
    bool build(const BuildParams& params, const GeometryInput& geometry,
               const PostprocessingFlags& flags = PostprocessingFlags());

private:
    dtNavMesh* navmesh;
    rcPolyMesh* polyMesh;
    rcPolyMeshDetail* polyMeshDetail;
    dtNavMeshQuery* navQuery;

    void cleanup();
};

} // namespace com
} // namespace bl

#endif
