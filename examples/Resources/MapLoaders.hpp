#ifndef MAPLOADERS_HPP
#define MAPLOADERS_HPP

#include "Map.hpp"
#include <BLIB/Resources/Loader.hpp>

struct MapLoaderDevMode : public bl::resource::LoaderBase<Map> {
    virtual bl::resource::Resource<Map>::Ref load(const std::string& uri, const char*, std::size_t,
                                                  std::istream& input) override {
        bl::resource::Resource<Map>::Ref result{new Map()};
        if (!result->loadFromJson(input)) { BL_LOG_ERROR << "Failed to load map: " << uri; }
        return result;
    }
};

struct MapLoaderProdMode : public bl::resource::LoaderBase<Map> {
    virtual bl::resource::Resource<Map>::Ref load(const std::string& uri, const char*, std::size_t,
                                                  std::istream& input) override {
        bl::resource::Resource<Map>::Ref result{new Map()};
        if (!result->loadFromBinary(input)) { BL_LOG_ERROR << "Failed to load map: " << uri; }
        return result;
    }
};

#endif
