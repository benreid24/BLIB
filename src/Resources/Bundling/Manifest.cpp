#include <BLIB/Resources/Bundling/Manifest.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
using Serializer = serial::binary::Serializer<Manifest>;

Manifest::Manifest(const std::string& bundleDir)
: manifestFile(util::FileUtil::joinPath(bundleDir, "manifest.bm")) {}

const std::string& Manifest::getBundleForFile(const std::string& path) const {
    static const std::string empty;
    const auto it = manifest.find(path);
    return it != manifest.end() ? it->second : empty;
}

bool Manifest::containsFile(const std::string& path) const {
    const auto it = manifest.find(path);
    return it != manifest.end();
}

void Manifest::registerFileBundle(const std::string& path, const std::string& bundlePath) {
    manifest[path] = bundlePath;
}

bool Manifest::load() {
    manifest.clear();
    serial::binary::InputFile input(manifestFile);
    return Serializer::deserialize(input, *this);
}

bool Manifest::save() const {
    serial::binary::OutputFile output(manifestFile);
    return Serializer::serialize(output, *this);
}

} // namespace bundle
} // namespace resource
} // namespace bl
