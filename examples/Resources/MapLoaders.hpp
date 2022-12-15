#ifndef MAPLOADERS_HPP
#define MAPLOADERS_HPP

#include "Map.hpp"
#include <BLIB/Resources/Loader.hpp>

struct MapLoaderDevMode : public bl::resource::LoaderBase<Map> {
    virtual bool load(const std::string& uri, const char*, std::size_t, std::istream& input,
                      Map& result) override {
        if (!result.loadFromJson(input)) {
            BL_LOG_ERROR << "Failed to load map: " << uri;
            return false;
        }
        return true;
    }
};

struct MapLoaderProdMode : public bl::resource::LoaderBase<Map> {
    virtual bool load(const std::string& uri, const char*, std::size_t, std::istream& input,
                      Map& result) override {
        if (!result.loadFromBinary(input)) {
            BL_LOG_ERROR << "Failed to load map: " << uri;
            return false;
        }
        return true;
    }
};

#endif
