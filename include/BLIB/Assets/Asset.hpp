#ifndef BLIB_ASSETS_ASSET_HPP
#define BLIB_ASSETS_ASSET_HPP

#include <BLIB/Assets/Metadata.hpp>
#include <BLIB/Util/UUID.hpp>

namespace bl
{
/// Contains the Asset system
namespace as
{
class Asset {
public:
    Asset();

    const util::UUID& getUUID() const { return uuid; }

    const std::string& getType() const { return type; }

    Metadata& getMetadata() { return metadata; }

    const Metadata& getMetadata() const { return metadata; }

private:
    util::UUID uuid;
    std::string type;
    Metadata metadata;
};

} // namespace as
} // namespace bl

#endif
