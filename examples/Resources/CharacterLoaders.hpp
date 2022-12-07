#ifndef CHARACTERLOADERS_HPP
#define CHARACTERLOADERS_HPP

#include "Character.hpp"
#include <BLIB/Resources/Loader.hpp>

struct CharacterLoaderDevMode : public bl::resource::LoaderBase<Character> {
    virtual bool load(const std::string& uri, const char*, std::size_t, std::istream& input,
                      Character& result) override {
        if (!result.loadFromJson(input)) {
            BL_LOG_ERROR << "Failed to load character: " << uri;
            return false;
        }
        return true;
    }
};

struct CharacterLoaderProdMode : public bl::resource::LoaderBase<Character> {
    virtual bool load(const std::string& uri, const char*, std::size_t, std::istream& input,
                      Character& result) override {
        if (!result.loadFromBinary(input)) {
            BL_LOG_ERROR << "Failed to load character: " << uri;
            return false;
        }
        return true;
    }
};

#endif
