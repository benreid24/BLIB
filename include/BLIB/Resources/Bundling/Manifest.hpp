#ifndef BLIB_RESOURCES_BUNDLING_MANIFEST_HPP
#define BLIB_RESOURCES_BUNDLING_MANIFEST_HPP

#include <BLIB/Serialization.hpp>
#include <string>
#include <unordered_map>

namespace bl
{
namespace resource
{
namespace bundle
{
/**
 * @brief Top level manifest that maps original file paths to the bundle path they are in. Bundle
 *        paths in the manifest are relative to the manifest file itself
 *
 * @ingroup Bundling
 */
class Manifest {
public:
    /**
     * @brief Creates an empty manifest for the given bundle directory. Call load() to load existing
     *        manifests
     *
     * @param bundleDirectory The directory containing the bundles and manifest file
     */
    Manifest(const std::string& bundleDirectory);

    /**
     * @brief Returns the fully qualified path to the bundle containing the given path. Returns an
     *        empty string if the path is not found
     *
     * @param path Original path to the resource to get the bundle for
     * @return const std::string& File of the bundle containing the given path
     */
    const std::string& getBundleForFile(const std::string& path) const;

    /**
     * @brief Returns whether or not the given path is in the manifest
     *
     * @return True if the path is in the manifest, false otherwise
     */
    bool containsFile(const std::string& path) const;

    /**
     * @brief Registers the given path as being part of the given bundle
     *
     * @param path The path that is now in the bundle
     * @param bundleFile The bundle the original path is now in
     */
    void registerFileBundle(const std::string& path, const std::string& bundleFile);

    /**
     * @brief Loads the manifest from disk
     *
     * @return True if the manifest could be loaded, false otherwise
     */
    bool load();

    /**
     * @brief Saves the manifest file to disk
     *
     * @return True if the manifest could be saved, false otherwise
     */
    bool save() const;

private:
    const std::string manifestFile;
    std::unordered_map<std::string, std::string> manifest;

    friend struct serial::SerializableObject<Manifest>;
};

} // namespace bundle
} // namespace resource

namespace serial
{
template<>
struct SerializableObject<resource::bundle::Manifest> : public SerializableObjectBase {
    using T = resource::bundle::Manifest;

    SerializableField<1, T, std::unordered_map<std::string, std::string>> manifest;

    SerializableObject()
    : SerializableObjectBase("BundleManifest")
    , manifest("manifest", *this, &T::manifest, SerializableFieldBase::Required{}) {}
};
} // namespace serial

} // namespace bl

#endif
