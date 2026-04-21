#include <BLIB/Assets/Drivers/ModelDriver.hpp>

#include <BLIB/Models/Importer.hpp>

namespace bl
{
namespace asi
{
struct LinkData {
    std::string modelFilename;
};
} // namespace asi
namespace serial
{
template<>
struct SerializableObject<asi::LinkData> : public SerializableObjectBase {
    SerializableField<1, asi::LinkData, std::string> modelFilename;
    SerializableObject()
    : SerializableObjectBase("ModelDriverLinkData")
    , modelFilename("modelFilename", *this, &asi::LinkData::modelFilename,
                    SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace asi
{
bool ModelDriver::doCreate(const as::CreateContext& ctx, ModelPayload& payload) {
    if (ctx.getCustomData().getPath().empty()) { return false; }

    const std::string path = ctx.getFilesDirectory();

    // copy model file into files directory and create link file to track the name
    if (ctx.getMode() == as::Mode::Editor) {
        const std::string modelBasename =
            util::FileUtil::getBaseName(ctx.getCustomData().getPath());
        const std::string modelPath = util::FileUtil::joinPath(path, modelBasename);
        if (!util::FileUtil::copyFile(ctx.getCustomData().getPath(), modelPath)) { return false; }

        stream::OutputStream output;
        if (!ctx.setupWriteStream("link.json", output)) { return false; }
        if (!serial::json::Serializer<LinkData>::serializeStream(
                output, LinkData{modelBasename}, 4, 0)) {
            return false;
        }
    }

    mdl::Importer importer;
    if (!importer.import(ctx.getCustomData().getPath(), payload.get())) { return false; }

    // copy all dependencies into the asset folder
    if (ctx.getMode() == as::Mode::Editor) {
        bool allSuccess = true;
        payload.get().visitDependencies(
            [&path, &allSuccess](const std::string& src) -> std::string {
                const std::string dst =
                    util::FileUtil::joinPath(path, util::FileUtil::getBaseName(src));
                if (!util::FileUtil::copyFile(src, dst)) { allSuccess = false; }
                return dst;
            });
        return allSuccess;
    }

    return true;
}

bool ModelDriver::doRead(const as::ReadContext& ctx, ModelPayload& payload) {
    if (ctx.getMode() == as::Mode::Editor) {
        stream::InputStream linkInput;
        if (!ctx.setupReadStream("link.json", linkInput)) { return false; }
        LinkData linkData;
        if (!serial::json::Serializer<LinkData>::deserializeStream(linkInput, linkData)) {
            return false;
        }

        const std::string modelFile =
            util::FileUtil::joinPath(ctx.getFilesDirectory(), linkData.modelFilename);
        mdl::Importer importer;
        if (!importer.import(modelFile, payload.get())) { return false; }

        return true;
    }
    else {
        // TODO - deserialize directly. how are textures stored?
        // TODO - need to either embed textures or make model asset aware and make assets
        return false;
    }
}

bool ModelDriver::doWrite(const as::WriteContext& ctx, const ModelPayload& payload) {
    if (ctx.getMode() == as::Mode::Editor) {
        BL_LOG_ERROR << "ModelDriver does not support writing in editor mode";
        return false;
    }

    // TODO - serialize directly. how are textures stored?
    return false;
}

} // namespace asi
} // namespace bl
