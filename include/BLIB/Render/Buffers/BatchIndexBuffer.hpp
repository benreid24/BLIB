#ifndef BLIB_RENDER_BUFFERS_BATCHINDEXBUFFER_HPP
#define BLIB_RENDER_BUFFERS_BATCHINDEXBUFFER_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <list>
#include <memory>

namespace bl
{
namespace rc
{
namespace buf
{
/**
 * @brief Wrapper around IndexBufferT that allows allocations to be made out of it. Intended use is
 *        for batching multiple simple renderables into a single draw call
 *
 * @tparam T The vertex type to use
 * @ingroup Renderer
 */
template<typename T>
class BatchIndexBufferT {
public:
    static constexpr VkIndexType IndexType = IndexBufferT<T>::IndexType;

    /**
     * @brief Basic info defining a sub-allocation in the index buffer
     */
    struct AllocInfo {
        std::uint32_t refCount;
        std::uint32_t vertexStart;
        std::uint32_t vertexSize;
        std::uint32_t indexStart;
        std::uint32_t indexSize;
    };

    /**
     * @brief Handle to an allocation from the index buffer
     */
    class AllocHandle {
    public:
        /**
         * @brief Creates an invalid handle
         */
        AllocHandle();

        /**
         * @brief Copies the allocation handle
         *
         * @param copy The handle to copy
         */
        AllocHandle(const AllocHandle& copy);

        /**
         * @brief Moves from the given handle
         *
         * @param move The handle to move from. Is invalidated
         */
        AllocHandle(AllocHandle&& move);

        /**
         * @brief Releases the handle if still owned
         */
        ~AllocHandle();

        /**
         * @brief Copies the allocation handle
         *
         * @param copy The handle to copy
         * @return A reference to this handle
         */
        AllocHandle& operator=(const AllocHandle& copy);

        /**
         * @brief Moves from the given handle
         *
         * @param move The handle to move from. Is invalidated
         * @return A reference to this handle
         */
        AllocHandle& operator=(AllocHandle&& move);

        /**
         * @brief Triggers the underlying index buffer to transfer. Call when contents are modified
         */
        void commit();

        /**
         * @brief Releases this handle. Frees the allocation if this is the last handle
         *
         * @return True if the allocation was freed, false if there are other handles
         */
        bool release();

        /**
         * @brief Invalidates this handle without releasing. Useful if the allocated vertices should
         *        outlive the handle used to create them
         */
        void orphan();

        /**
         * @brief Returns the vertices for writing. Do not store pointers as they may be invalidated
         */
        T* getVertices();

        /**
         * @brief Returns the indices for writing. Do not store pointers as they may be invalidated
         */
        std::uint32_t* getIndices();

        /**
         * @brief Returns the allocation parameters
         */
        const AllocInfo& getInfo() const;

        /**
         * @brief Returns whether or not this handle points to a valid allocation
         */
        bool isValid() const;

    private:
        BatchIndexBufferT* owner;
        typename std::list<AllocInfo>::iterator alloc;
        std::shared_ptr<bool> parentAlive;

        AllocHandle(BatchIndexBufferT& owner, const std::shared_ptr<bool>& pflag,
                    typename std::list<AllocInfo>::iterator alloc);
        void incRef();

        friend class BatchIndexBufferT;
    };

    /**
     * @brief Creates an empty index buffer
     */
    BatchIndexBufferT();

    /**
     * @brief Destroys the buffer using deferDestroy()
     */
    virtual ~BatchIndexBufferT();

    /**
     * @brief Creates the batched index buffer with the given initial capacities
     *
     * @param vulkanState The renderer Vulkan state
     * @param initialVertexCount The starting number of vertices to allocate
     * @param initialIndexCount The starting number of indices to allocate
     */
    void create(vk::VulkanLayer& vulkanState, std::uint32_t initialVertexCount,
                std::uint32_t initialIndexCount);

    /**
     * @brief Allocates the requested number of vertices and indices from the buffer. Invalidates
     *        the draw parameters
     *
     * @param vertexCount The number of vertices to allocate
     * @param indexCount The number of indices to allocate
     * @return A handle to the new allocation
     */
    AllocHandle allocate(std::uint32_t vertexCount, std::uint32_t indexCount);

    /**
     * @brief Frees both the vertex buffer and index buffer
     */
    void destroy();

    /**
     * @brief Queues the buffer to be erased in a deferred manner
     */
    void deferDestruction();

    /**
     * @brief Builds and returns the parameters required to render the index buffer
     */
    prim::DrawParameters getDrawParameters() const;

    /**
     * @brief Returns the number of in-use indices in the buffer
     */
    constexpr std::uint32_t indexCount() const;

    /**
     * @brief Returns the number of in-use vertices in the buffer
     */
    constexpr std::uint32_t vertexCount() const;

    /**
     * @brief Returns the total number of indices in the buffer
     */
    constexpr std::uint32_t indexCapacity() const;

    /**
     * @brief Returns the total number of vertices in the buffer
     */
    constexpr std::uint32_t vertexCapacity() const;

    /**
     * @brief Returns the Vulkan handle of the vertex buffer
     */
    constexpr VkBuffer vertexBufferHandle() const;

    /**
     * @brief Returns the Vulkan handle of the index buffer
     */
    constexpr VkBuffer indexBufferHandle() const;

    /**
     * @brief Provides const access to the underlying index buffer
     */
    const IndexBufferT<T>& getIndexBuffer() const;

    /**
     * @brief Manually commit the vertices to the GPU
     */
    void commit();

private:
    IndexBufferT<T> storage;
    std::uint32_t usedVertices;
    std::uint32_t usedIndices;
    std::list<AllocInfo> allocations;
    std::shared_ptr<bool> alive;

    void release(typename std::list<AllocInfo>::iterator alloc);
};

/**
 * @brief Convenience typedef for the standard vertex type
 *
 * @ingroup Renderer
 */
using BatchIndexBuffer = BatchIndexBufferT<prim::Vertex>;

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
BatchIndexBufferT<T>::BatchIndexBufferT()
: usedVertices(0)
, usedIndices(0)
, alive(std::make_shared<bool>(true)) {}

template<typename T>
BatchIndexBufferT<T>::~BatchIndexBufferT() {
    *alive = false;
    storage.deferDestruction();
}

template<typename T>
void BatchIndexBufferT<T>::create(vk::VulkanLayer& vulkanState, std::uint32_t initialVertexCount,
                                  std::uint32_t initialIndexCount) {
    deferDestruction();
    storage.create(vulkanState, initialVertexCount, initialIndexCount);
}

template<typename T>
typename BatchIndexBufferT<T>::AllocHandle BatchIndexBufferT<T>::allocate(std::uint32_t reqv,
                                                                          std::uint32_t reqi) {
    const bool needVertices = usedVertices + reqv > vertexCapacity();
    const bool needIndices  = usedIndices + reqi > indexCapacity();
    if (needVertices || needIndices) {
        const std::uint32_t vc =
            needVertices ? std::max(vertexCapacity() * 2, usedVertices + reqv) : vertexCapacity();
        const std::uint32_t ic =
            needIndices ? std::max(indexCapacity() * 2, usedIndices + reqi) : indexCapacity();
        storage.ensureSize(vc, ic);
    }

    auto it           = allocations.emplace(allocations.end(), AllocInfo{});
    AllocInfo& alloc  = *it;
    alloc.indexStart  = usedIndices;
    alloc.vertexStart = usedVertices;
    alloc.vertexSize  = reqv;
    alloc.indexSize   = reqi;

    usedVertices += reqv;
    usedIndices += reqi;
    storage.configureWriteRange(0, usedVertices, 0, usedIndices);

    return {*this, alive, it};
}

template<typename T>
void BatchIndexBufferT<T>::destroy() {
    *alive       = false;
    alive        = std::make_shared<bool>(true);
    usedIndices  = 0;
    usedVertices = 0;
    allocations.clear();
    storage.destroy();
}

template<typename T>
void BatchIndexBufferT<T>::deferDestruction() {
    *alive       = false;
    alive        = std::make_shared<bool>(true);
    usedIndices  = 0;
    usedVertices = 0;
    allocations.clear();
    storage.deferDestruction();
}

template<typename T>
prim::DrawParameters BatchIndexBufferT<T>::getDrawParameters() const {
    prim::DrawParameters params = storage.getDrawParameters();
    params.indexCount           = usedIndices;
    return params;
}

template<typename T>
constexpr std::uint32_t BatchIndexBufferT<T>::indexCount() const {
    return usedIndices;
}

template<typename T>
constexpr std::uint32_t BatchIndexBufferT<T>::vertexCount() const {
    return usedVertices;
}

template<typename T>
constexpr std::uint32_t BatchIndexBufferT<T>::indexCapacity() const {
    return storage.indexCount();
}

template<typename T>
constexpr std::uint32_t BatchIndexBufferT<T>::vertexCapacity() const {
    return storage.vertexCount();
}

template<typename T>
constexpr VkBuffer BatchIndexBufferT<T>::vertexBufferHandle() const {
    return storage.vertexBufferHandle();
}

template<typename T>
constexpr VkBuffer BatchIndexBufferT<T>::indexBufferHandle() const {
    return storage.indexBufferHandle();
}

template<typename T>
const IndexBufferT<T>& BatchIndexBufferT<T>::getIndexBuffer() const {
    return storage;
}

template<typename T>
void BatchIndexBufferT<T>::release(typename std::list<AllocInfo>::iterator it) {
    const AllocInfo alloc = *it;
    allocations.erase(it);

    // copy vertices to the left
    const auto vertexStartIt = storage.vertices().begin() + alloc.vertexStart;
    const auto nextVertexIt  = vertexStartIt + alloc.vertexSize;
    std::copy(nextVertexIt, storage.vertices().end(), vertexStartIt);

    // copy indices to the left
    const auto indexStartIt = storage.indices().begin() + alloc.indexStart;
    const auto nextIndexIt  = indexStartIt + alloc.indexSize;
    std::copy(nextIndexIt, storage.indices().end(), indexStartIt);

    // update indices of affected allocations
    for (AllocInfo& a : allocations) {
        if (a.vertexStart > alloc.vertexStart) { a.vertexStart -= alloc.vertexSize; }
        if (a.indexStart > alloc.indexStart) { a.indexStart -= alloc.indexSize; }
    }

    // update values of affected indices
    for (auto& i : storage.indices()) {
        if (i >= alloc.vertexStart) { i -= alloc.vertexSize; }
    }

    // update metadata and commit
    usedVertices -= alloc.vertexSize;
    usedIndices -= alloc.indexSize;
    storage.configureWriteRange(0, usedVertices, 0, usedIndices);
    commit();
}

template<typename T>
void BatchIndexBufferT<T>::commit() {
    if (storage.queueTransfer(tfr::Transferable::SyncRequirement::Immediate)) {
        storage.insertBarrierBeforeWrite();
    }
}

template<typename T>
BatchIndexBufferT<T>::AllocHandle::AllocHandle(BatchIndexBufferT& owner,
                                               const std::shared_ptr<bool>& pflag,
                                               typename std::list<AllocInfo>::iterator alloc)
: owner(&owner)
, alloc(alloc)
, parentAlive(pflag) {
    incRef();
}

template<typename T>
void BatchIndexBufferT<T>::AllocHandle::incRef() {
    if (isValid()) { alloc->refCount += 1; }
}

template<typename T>
BatchIndexBufferT<T>::AllocHandle::AllocHandle()
: owner(nullptr) {}

template<typename T>
BatchIndexBufferT<T>::AllocHandle::AllocHandle(const AllocHandle& copy)
: owner(copy.owner)
, alloc(copy.alloc)
, parentAlive(copy.parentAlive) {
    incRef();
}

template<typename T>
BatchIndexBufferT<T>::AllocHandle::AllocHandle(AllocHandle&& move)
: owner(move.owner)
, alloc(move.alloc)
, parentAlive(std::move(move.parentAlive)) {
    move.owner = nullptr;
    move.alloc = owner->allocations.end();
}

template<typename T>
BatchIndexBufferT<T>::AllocHandle::~AllocHandle() {
    release();
}

template<typename T>
typename BatchIndexBufferT<T>::AllocHandle& BatchIndexBufferT<T>::AllocHandle::operator=(
    const AllocHandle& copy) {
    release();
    owner       = copy.owner;
    alloc       = copy.alloc;
    parentAlive = copy.parentAlive;
    incRef();
    return *this;
}

template<typename T>
typename BatchIndexBufferT<T>::AllocHandle& BatchIndexBufferT<T>::AllocHandle::operator=(
    AllocHandle&& move) {
    release();
    owner       = move.owner;
    alloc       = move.alloc;
    parentAlive = std::move(move.parentAlive);
    move.owner  = nullptr;
    move.alloc  = owner->allocations.end();
    return *this;
}

template<typename T>
void BatchIndexBufferT<T>::AllocHandle::commit() {
    if (isValid()) { owner->commit(); }
}

template<typename T>
bool BatchIndexBufferT<T>::AllocHandle::release() {
    bool r = false;
    if (isValid()) {
        parentAlive.reset();
        --alloc->refCount;
        if (alloc->refCount == 0) {
            owner->release(alloc);
            r = true;
        }
        alloc = owner->allocations.end();
        owner = nullptr;
    }
    return r;
}

template<typename T>
void BatchIndexBufferT<T>::AllocHandle::orphan() {
    parentAlive.reset();
    alloc = owner->allocations.end();
    owner = nullptr;
}

template<typename T>
T* BatchIndexBufferT<T>::AllocHandle::getVertices() {
    return &owner->storage.vertices()[alloc->vertexStart];
}

template<typename T>
std::uint32_t* BatchIndexBufferT<T>::AllocHandle::getIndices() {
    return &owner->storage.indices()[alloc->indexStart];
}

template<typename T>
const typename BatchIndexBufferT<T>::AllocInfo& BatchIndexBufferT<T>::AllocHandle::getInfo() const {
    return *alloc;
}

template<typename T>
bool BatchIndexBufferT<T>::AllocHandle::isValid() const {
    return owner != nullptr && parentAlive && *parentAlive && alloc != owner->allocations.end();
}

} // namespace buf
} // namespace rc
} // namespace bl

#endif
