#ifndef CHARACTERHANDLER_HPP
#define CHARACTERHANDLER_HPP

#include "Character.hpp"
#include <BLIB/Resources.hpp>

struct CharacterHandler : public bl::resource::bundle::FileHandler {
    virtual bool processFile(const std::string& path, std::ostream& output,
                             bl::resource::bundle::FileHandlerContext& ctx) override {
        Character c;
        if (!c.loadFromJsonFile(path)) return false;
        if (!c.saveToBinary(output)) return false;

        // in some cases data files may combine with others and we do not want to bundle the sidecar
        // files that we combined with. we can signal the bundler to exclude them here. If sidecar
        // files were already bundled by a prior BundleSource they will still get bundled so care
        // must be taken to order the sources properly
        ctx.addExcludeFile(bl::util::FileUtil::joinPath("game_data/character_extra",
                                                        bl::util::FileUtil::getFilename(path)));
        return true;
    }
};

#endif
