#include <BLIB/Engine/Properties.hpp>

#include <vector>

namespace bl
{
namespace engine
{
namespace
{
priv::PropertyBase* properties[512];
std::uint16_t nprops = 0;
} // namespace

void Properties::syncToConfig() {
    for (std::uint16_t i = 0; i < nprops; ++i) { properties[i]->syncToConfig(); }
}

void Properties::syncFromConfig() {
    for (std::uint16_t i = 0; i < nprops; ++i) { properties[i]->syncFromConfig(); }
}

void Properties::addProperty(priv::PropertyBase* prop) {
    properties[nprops] = prop;
    ++nprops;
}

void Properties::removeProperty(priv::PropertyBase* prop) {
    if (nprops > 1) {
        for (std::uint16_t i = 0; i < nprops; ++i) {
            if (properties[i] == prop) {
                properties[i] = properties[nprops - 1];
                --nprops;
                return;
            }
        }
    }
    else { nprops = 0; }
}

} // namespace engine
} // namespace bl
