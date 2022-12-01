#ifndef MAPHANDLER_HPP
#define MAPHANDLER_HPP

#include "Map.hpp"
#include <BLIB/Resources.hpp>

struct MapHandler : public bl::resource::bundle::FileHandler {
    virtual bool processFile(const std::string& path, std::ostream& output,
                             bl::resource::bundle::FileHandlerContext& ctx) override {
        Map m;
        if (!m.loadFromJsonFile(path)) return false;
        if (!m.saveToBinary(output)) return false;

        // we can signal to the bundler to try and package our dependencies with us
        for (const std::string& npcFile : m.npcs) {
            ctx.addDependencyFile(bl::util::FileUtil::joinPath(Map::NpcPath, npcFile));
        }
        return true;
    }
};

#endif
