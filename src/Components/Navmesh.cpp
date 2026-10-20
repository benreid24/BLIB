#include <BLIB/Components/Navmesh.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Util/RAIICleanup.hpp>
#include <DetourNavMeshBuilder.h>
#include <cmath>

namespace bl
{
namespace com
{
namespace
{
class VertexAdaptor {
public:
    VertexAdaptor(glm::vec3* vertices, unsigned int stride, unsigned int count)
    : vertices(vertices)
    , ownsMemory(false) {
        if (stride != sizeof(glm::vec3)) {
            ownsMemory     = true;
            this->vertices = new glm::vec3[count];
            for (unsigned int i = 0; i < count; ++i) {
                this->vertices[i] = *reinterpret_cast<glm::vec3*>(
                    reinterpret_cast<std::uint8_t*>(vertices) + i * stride);
            }
        }
    }

    ~VertexAdaptor() {
        if (ownsMemory) { delete[] vertices; }
    }

    float* get() { return &vertices[0].x; }

private:
    glm::vec3* vertices;
    bool ownsMemory;
};
} // namespace

Navmesh::Navmesh()
: navmesh(nullptr)
, polyMesh(nullptr)
, polyMeshDetail(nullptr) {}

Navmesh::~Navmesh() { cleanup(); }

void Navmesh::cleanup() {
    if (navmesh) {
        dtFreeNavMesh(navmesh);
        navmesh = nullptr;
    }
    if (polyMesh) {
        rcFreePolyMesh(polyMesh);
        polyMesh = nullptr;
    }
    if (polyMeshDetail) {
        rcFreePolyMeshDetail(polyMeshDetail);
        polyMeshDetail = nullptr;
    }
}

bool Navmesh::build(const BuildParams& params, const GeometryInput& geometry,
                    const PostprocessingFlags& ppFlags) {
    cleanup();

    // TODO - make custom context?
    rcContext context;
    util::RAIICleanup cleaner(4);

    rcConfig config{};
    rcVcopy(config.bmin, &params.topLeftBound.x);
    rcVcopy(config.bmax, &params.bottomRightBound.x);
    config.cs                     = params.cellSize;
    config.ch                     = params.cellHeight;
    config.walkableSlopeAngle     = params.maxSlopeAngleDegrees;
    config.walkableHeight         = std::ceil(params.agentHeight / params.cellHeight);
    config.walkableClimb          = std::floor(params.agentMaxClimb / params.cellHeight);
    config.walkableRadius         = std::ceil(params.agentRadius / params.cellHeight);
    config.maxEdgeLen             = params.maxEdgeLength / params.cellSize;
    config.maxSimplificationError = params.maxEdgeError;
    config.minRegionArea          = params.minIslandArea / (params.cellSize * params.cellSize);
    config.mergeRegionArea        = params.mergeIslandArea / (params.cellSize * params.cellSize);
    config.maxVertsPerPoly        = 6;
    config.detailSampleDist       = params.detailSampleDistance;
    config.detailSampleMaxError   = params.detailSampleMaxError;
    rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);

    rcHeightfield* heightfield = rcAllocHeightfield();
    cleaner.add([heightfield]() { rcFreeHeightField(heightfield); });

    if (!rcCreateHeightfield(&context,
                             *heightfield,
                             config.width,
                             config.height,
                             config.bmin,
                             config.bmax,
                             config.cs,
                             config.ch)) {
        BL_LOG_ERROR << "Failed to create heightfield for navmesh build";
        return false;
    }

    const int* indices              = reinterpret_cast<const int*>(geometry.indices);
    const unsigned int numTriangles = geometry.indexCount / 3;
    VertexAdaptor vertices(geometry.vertices, geometry.vertexStride, geometry.vertexCount);
    unsigned char* walkableTriangles = new unsigned char[numTriangles];
    std::memset(walkableTriangles, 0, numTriangles);
    rcMarkWalkableTriangles(&context,
                            config.walkableSlopeAngle,
                            vertices.get(),
                            geometry.vertexCount,
                            indices,
                            numTriangles,
                            walkableTriangles);
    cleaner.add([walkableTriangles]() { delete[] walkableTriangles; });

    if (!rcRasterizeTriangles(&context,
                              vertices.get(),
                              walkableTriangles,
                              numTriangles,
                              *heightfield,
                              config.walkableClimb)) {
        BL_LOG_ERROR << "Failed to rasterize triangles for navmesh build";
        return false;
    }

    if (ppFlags.filterLowHangingObstacles) {
        rcFilterLowHangingWalkableObstacles(&context, config.walkableClimb, *heightfield);
    }
    if (ppFlags.filterLedgeSpans) {
        rcFilterLedgeSpans(&context, config.walkableHeight, config.walkableClimb, *heightfield);
    }
    if (ppFlags.filterWalkableLowHeightSpans) {
        rcFilterWalkableLowHeightSpans(&context, config.walkableHeight, *heightfield);
    }

    rcCompactHeightfield* compactHeightfield = rcAllocCompactHeightfield();
    cleaner.add([compactHeightfield]() { rcFreeCompactHeightfield(compactHeightfield); });
    if (!rcBuildCompactHeightfield(&context,
                                   config.walkableHeight,
                                   config.walkableClimb,
                                   *heightfield,
                                   *compactHeightfield)) {
        BL_LOG_ERROR << "Failed to build compact heightfield for navmesh build";
        return false;
    }

    if (!rcErodeWalkableArea(&context, config.walkableRadius, *compactHeightfield)) {
        BL_LOG_ERROR << "Failed to erode walkable area";
        return false;
    }

    // TODO - convex polygon area marking? rcMarkConvexPolyArea

    // TODO - parameterize partition method? other options are rcBuildRegionsMonotone or
    // rcBuildLayerRegions
    if (!rcBuildDistanceField(&context, *compactHeightfield)) {
        BL_LOG_ERROR << "Failed to build distance field";
        return false;
    }
    if (!rcBuildRegions(&context,
                        *compactHeightfield,
                        config.borderSize,
                        config.minRegionArea,
                        config.mergeRegionArea)) {
        BL_LOG_ERROR << "Failed to build regions";
        return false;
    }

    rcContourSet* contourSet = rcAllocContourSet();
    cleaner.add([contourSet]() { rcFreeContourSet(contourSet); });
    if (!rcBuildContours(&context,
                         *compactHeightfield,
                         config.maxSimplificationError,
                         config.maxEdgeLen,
                         *contourSet)) {
        BL_LOG_ERROR << "Failed to build contours";
        return false;
    }

    polyMesh = rcAllocPolyMesh();
    cleaner.add([this]() { rcFreePolyMesh(polyMesh); });
    if (!rcBuildPolyMesh(&context, *contourSet, config.maxVertsPerPoly, *polyMesh)) {
        BL_LOG_ERROR << "Failed to build poly mesh";
        return false;
    }

    polyMeshDetail = rcAllocPolyMeshDetail();
    cleaner.add([this]() { rcFreePolyMeshDetail(polyMeshDetail); });
    if (!rcBuildPolyMeshDetail(&context,
                               *polyMesh,
                               *compactHeightfield,
                               config.detailSampleDist,
                               config.detailSampleMaxError,
                               *polyMeshDetail)) {
        BL_LOG_ERROR << "Failed to build poly mesh detail";
        return false;
    }

    dtNavMeshCreateParams navParams{};
    navParams.verts            = polyMesh->verts;
    navParams.vertCount        = polyMesh->nverts;
    navParams.polys            = polyMesh->polys;
    navParams.polyAreas        = polyMesh->areas;
    navParams.polyFlags        = polyMesh->flags;
    navParams.polyCount        = polyMesh->npolys;
    navParams.nvp              = polyMesh->nvp;
    navParams.detailMeshes     = polyMeshDetail->meshes;
    navParams.detailVerts      = polyMeshDetail->verts;
    navParams.detailVertsCount = polyMeshDetail->nverts;
    navParams.detailTris       = polyMeshDetail->tris;
    navParams.detailTriCount   = polyMeshDetail->ntris;
    navParams.walkableHeight   = params.agentHeight;
    navParams.walkableRadius   = params.agentRadius;
    navParams.walkableClimb    = params.agentMaxClimb;
    rcVcopy(navParams.bmin, polyMesh->bmin);
    rcVcopy(navParams.bmax, polyMesh->bmax);
    navParams.cs          = config.cs;
    navParams.ch          = config.ch;
    navParams.buildBvTree = true;

    unsigned char* navData = 0;
    int navDataSize        = 0;
    if (!dtCreateNavMeshData(&navParams, &navData, &navDataSize)) {
        BL_LOG_ERROR << "Failed to create navmesh data";
        return false;
    }

    navmesh = dtAllocNavMesh();
    cleaner.add([this]() {
        dtFreeNavMesh(navmesh);
        navmesh = nullptr;
    });

    dtStatus status = navmesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
    if (dtStatusFailed(status)) {
        dtFree(navData);
        BL_LOG_ERROR << "Failed to init navmesh";
        return false;
    }

    status = navQuery->init(navmesh, 2048); // TODO - parameterize max nodes?
    if (dtStatusFailed(status)) {
        BL_LOG_ERROR << "Failed to init navmesh query";
        return false;
    }

    cleaner.disarm();
    delete[] walkableTriangles;
    rcFreeCompactHeightfield(compactHeightfield);
    rcFreeHeightField(heightfield);
    rcFreeContourSet(contourSet);
    return true;
}

} // namespace com
} // namespace bl
