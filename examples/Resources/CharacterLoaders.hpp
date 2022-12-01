#ifndef CHARACTERLOADERS_HPP
#define CHARACTERLOADERS_HPP

#include "Character.hpp"
#include <BLIB/Resources/Loader.hpp>

struct CharacterLoaderDevMode : public bl::resource::LoaderBase<Character> {
    virtual bl::resource::Resource<Character>::Ref load(const std::string& uri,
                                                        const std::vector<char>&,
                                                        std::istream& input) override {
        bl::resource::Resource<Character>::Ref result{new Character()};
        if (!result->loadFromJson(input)) { BL_LOG_ERROR << "Failed to load character: " << uri; }
        return result;
    }
};

struct CharacterLoaderProdMode : public bl::resource::LoaderBase<Character> {
    virtual bl::resource::Resource<Character>::Ref load(const std::string& uri,
                                                        const std::vector<char>&,
                                                        std::istream& input) override {
        bl::resource::Resource<Character>::Ref result{new Character()};
        if (!result->loadFromBinary(input)) { BL_LOG_ERROR << "Failed to load character: " << uri; }
        return result;
    }
};

#endif