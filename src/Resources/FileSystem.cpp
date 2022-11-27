#include <BLIB/Resources/FileSystem.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <fstream>

namespace bl
{
namespace resource
{
FileSystem& FileSystem::get() {
    static FileSystem fs;
    return fs;
}

bool FileSystem::useBundle(const std::string&) {
    // TODO
    return false;
}

bool FileSystem::getData(const std::string& path, std::vector<char>& data) {
    // TODO - load from bundle if using bundle
    std::ifstream input(path.c_str(), std::ios::in | std::ios::binary);
    if (!input.good()) return false;
    input.seekg(0, std::ios_base::end);
    const std::streampos fileSize = input.tellg();
    data.resize(fileSize);
    input.seekg(0, std::ios_base::beg);
    input.read(data.data(), fileSize);
    return true;
}

bool FileSystem::resourceExists(const std::string& path) {
    // TODO - check in bundle if using bundle
    return util::FileUtil::exists(path);
}

} // namespace resource
} // namespace bl
