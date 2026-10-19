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
class Navmesh {
public:
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

    struct GeometryInput {
        glm::vec3* vertices;
        unsigned int vertexStride = sizeof(glm::vec3);
        unsigned int vertexCount;
        std::uint32_t* indices;
        unsigned int indexCount;
    };

    struct PostprocessingFlags {
        bool filterLowHangingObstacles    = false;
        bool filterLedgeSpans             = false;
        bool filterWalkableLowHeightSpans = false;
    };
    ;

    Navmesh();

    ~Navmesh();

    bool build(const BuildParams& params, const GeometryInput& geometry,
               const PostprocessingFlags& flags = PostprocessingFlags());

private:
    dtNavMesh* navmesh;
    dtNavMeshQuery* navQuery;
};

} // namespace com
} // namespace bl

#endif
