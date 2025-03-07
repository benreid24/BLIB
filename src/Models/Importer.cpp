#include <BLIB/Models/Importer.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/Logger.hpp>
#include <assimp/postprocess.h>
#include <fstream>

namespace bl
{
namespace mdl
{
namespace
{
constexpr unsigned int PostProcessing =
    aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs |
    aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights | aiProcess_ImproveCacheLocality |
    aiProcess_RemoveRedundantMaterials | aiProcess_PopulateArmatureData | aiProcess_GenUVCoords |
    aiProcess_OptimizeMeshes;

class AssimpIOSystem;

class BundleStream : public Assimp::IOStream {
public:
    virtual ~BundleStream() { resource::FileSystem::purgePersistentData(path); }

    virtual size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override {
        for (unsigned int i = 0; i < pCount; ++i) {
            if (head + pSize <= len) {
                std::memcpy(pvBuffer, data + head, pSize);
                head += pSize;
                pvBuffer = static_cast<char*>(pvBuffer) + pSize;
            }
            else { return i; }
        }
        return pCount;
    }

    virtual size_t Write(const void*, size_t, size_t) override {
        BL_LOG_ERROR << "Trying to output to read-only bundle stream";
        return 0;
    }

    virtual aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override {
        switch (pOrigin) {
        case aiOrigin_SET:
            head = pOffset;
            if (head > len) { return aiReturn_FAILURE; }
            break;
        case aiOrigin_CUR:
            head += pOffset;
            if (head > len) { return aiReturn_FAILURE; }
            break;
        case aiOrigin_END:
            if (pOffset > len) { return aiReturn_FAILURE; }
            head = len - pOffset;
            break;
        default:
            return aiReturn_FAILURE;
        }

        return aiReturn_SUCCESS;
    }

    virtual size_t Tell() const override { return head; }

    virtual size_t FileSize() const override { return len; }

    virtual void Flush() override {
        // noop
    }

private:
    std::string path;
    char* data;
    std::size_t len;
    std::size_t head;

    BundleStream(const char* p)
    : path(p)
    , head(0) {
        resource::FileSystem::getData(path, &data, len);
    }

    friend class AssimpIOSystem;
};

class AssimpIOSystem : public Assimp::IOSystem {
public:
    AssimpIOSystem() = default;

    virtual ~AssimpIOSystem() = default;

    virtual bool Exists(const char* pFile) const override {
        return resource::FileSystem::resourceExists(pFile);
    }

    virtual char getOsSeparator() const override { return '/'; }

    virtual Assimp::IOStream* Open(const char* pFile, const char*) override {
        return new BundleStream(pFile);
    }

    virtual void Close(Assimp::IOStream* pFile) override { delete pFile; }
};

class DebugLogger : public Assimp::LogStream {
public:
    virtual void write(const char* message) override { BL_LOG_DEBUG << message; }
};

class InfoLogger : public Assimp::LogStream {
public:
    virtual void write(const char* message) override { BL_LOG_INFO << message; }
};

class WarnLogger : public Assimp::LogStream {
public:
    virtual void write(const char* message) override { BL_LOG_WARN << message; }
};

class ErrorLogger : public Assimp::LogStream {
public:
    virtual void write(const char* message) override { BL_LOG_ERROR << message; }
};

} // namespace

Importer::Importer() {
    Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL);
    Assimp::DefaultLogger::get()->attachStream(new DebugLogger(), Assimp::Logger::Debugging);
    Assimp::DefaultLogger::get()->attachStream(new InfoLogger(), Assimp::Logger::Info);
    Assimp::DefaultLogger::get()->attachStream(new WarnLogger(), Assimp::Logger::Warn);
    Assimp::DefaultLogger::get()->attachStream(new ErrorLogger(), Assimp::Logger::Err);

    importer.SetIOHandler(new AssimpIOSystem());
}

bool Importer::import(const std::string& path, Model& result) {
    const aiScene* scene = importer.ReadFile(path, PostProcessing);
    if (!scene) {
        BL_LOG_ERROR << "Failed to load model: " << importer.GetErrorString();
        return false;
    }
    result.populate(scene);
    importer.FreeScene();
    return true;
}

bool Importer::import(const char* buffer, std::size_t len, Model& result,
                      const std::string& pathHint) {
    if (!pathHint.empty()) {
        const std::string dir = util::FileUtil::getPath(pathHint);
        importer.GetIOHandler()->PushDirectory(dir.c_str());
    }
    const aiScene* scene = importer.ReadFileFromMemory(buffer, len, PostProcessing);
    if (!scene) {
        BL_LOG_ERROR << "Failed to load model: " << importer.GetErrorString();
        importer.GetIOHandler()->PopDirectory();
        return false;
    }
    result.populate(scene);
    importer.GetIOHandler()->PopDirectory();
    importer.FreeScene();
    return true;
}

} // namespace mdl
} // namespace bl
