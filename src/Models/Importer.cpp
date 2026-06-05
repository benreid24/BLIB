#include <BLIB/Models/Importer.hpp>

#include <BLIB/Assets/Builtin/ModelPayload.hpp>
#include <BLIB/Logging.hpp>
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
    aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace |
    aiProcess_LimitBoneWeights | aiProcess_RemoveRedundantMaterials |
    aiProcess_PopulateArmatureData | aiProcess_GenUVCoords | aiProcess_OptimizeMeshes;

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
}

bool Importer::import(const std::string& path, Model& result) {
    const aiScene* scene = importer.ReadFile(path, PostProcessing);
    if (!scene) {
        BL_LOG_ERROR << "Failed to load model: " << importer.GetErrorString();
        return false;
    }
    result.populate(scene, util::FileUtil::getPath(path));
    importer.FreeScene();
    return true;
}

bool Importer::import(const char* buffer, std::size_t len, Model& result,
                      const std::string& pathHint) {
    const std::string dir = util::FileUtil::getPath(pathHint);
    if (!pathHint.empty()) { importer.GetIOHandler()->PushDirectory(dir.c_str()); }
    const aiScene* scene = importer.ReadFileFromMemory(
        buffer, len, PostProcessing, util::FileUtil::getFilename(pathHint).c_str());
    if (!scene) {
        BL_LOG_ERROR << "Failed to load model: " << importer.GetErrorString();
        importer.GetIOHandler()->PopDirectory();
        return false;
    }
    result.populate(scene, dir);
    importer.GetIOHandler()->PopDirectory();
    importer.FreeScene();
    return true;
}

} // namespace mdl
} // namespace bl
