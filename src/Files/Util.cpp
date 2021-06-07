#include <BLIB/Files/Util.hpp>

#include <BLIB/Util/Random.hpp>
#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace bl
{
namespace file
{
namespace
{
void createDir(const std::string& path) {
#ifdef _WIN32
    mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}
} // namespace

bool Util::exists(const std::string& file) {
    std::ifstream test(file.c_str());
    return test.good();
}

bool Util::directoryExists(const std::string& file) {
    struct stat info;
    if (stat(file.c_str(), &info) != 0) return false;
    return info.st_mode & S_IFDIR;
}

bool Util::isBigEndian() {
    union {
        uint32_t i;
        char c[4];
    } endTest = {0x01020304};
    return endTest.c[0] == 1;
}

std::string Util::getExtension(const std::string& file) {
    size_t i = file.find_last_of(".");
    if (i != std::string::npos) return file.substr(i + 1);
    i = file.find_last_of("/\\");
    if (i != std::string::npos) return file.substr(i + 1);
    return file;
}

std::string Util::getBaseName(const std::string& file) {
    std::string base = getFilename(file);
    size_t i         = base.find_last_of(".");
    if (i != std::string::npos) base.erase(i);
    return base;
}

std::string Util::getFilename(const std::string& file) {
    const size_t i = file.find_last_of("/\\");
    if (i != std::string::npos) return file.substr(i + 1);
    return file;
}

std::string Util::getPath(const std::string& file) {
    const size_t i = file.find_last_of("/\\");
    return file.substr(0, (i != std::string::npos) ? (i + 1) : (i));
}

std::string Util::joinPath(const std::string& l, const std::string& r) {
    if (l.empty()) return r;
    if (r.empty()) return l;

    size_t ls = std::string::npos;
    if (l[l.size() - 1] == '/' || l[l.size() - 1] == '\\') ls = l.size() - 1;

    size_t rs = 0;
    if (r[0] == '/' || r[0] == '\\') rs = 1;

    return l.substr(0, ls) + "/" + r.substr(rs);
}

std::string Util::genTempName(const std::string& path, const std::string& ext) {
    std::string file;
    do {
        std::stringstream ss;
        ss << "TEMP_" << std::hex << util::Random::get(1000, 10000000);
        if (!ext.empty()) {
            if (ext[0] != '.') ss << ".";
            ss << ext;
        }
        file = joinPath(path, ss.str());
    } while (exists(file));
    return file;
}

void Util::copyFile(const std::string& src, const std::string& dest) {
    if (src == dest) return;

    std::ifstream source(src.c_str(), std::ios::binary);
    std::ofstream dst(dest.c_str(), std::ios::binary);

    std::istreambuf_iterator<char> begin_source(source);
    std::istreambuf_iterator<char> end_source;
    std::ostreambuf_iterator<char> begin_dest(dst);
    std::copy(begin_source, end_source, begin_dest);
}

bool Util::createDirectory(const std::string& path) {
    if (directoryExists(path)) return true;

    std::string cd;
    cd.reserve(path.size());
    for (unsigned int i = 0; i < path.size(); ++i) {
        if (path[i] == '/' || path[i] == '\\') { createDir(cd); }
        cd.push_back(path[i]);
    }
    createDir(cd);
    return directoryExists(cd);
}

std::vector<std::string> Util::listDirectory(const std::string& path, const std::string& ext,
                                             bool recursive) {
    if (path.empty()) return {};

    DIR* cd;
    struct dirent* cfile;
    std::vector<std::string> list;
    std::string folder = path;
    if (folder[folder.size() - 1] != '/' && folder[folder.size() - 1] != '\\')
        folder.push_back('/');

    cd = opendir(folder.c_str());
    if (cd != nullptr) {
        while ((cfile = readdir(cd))) {
            std::string file = cfile->d_name;
            if (file.find(".") != std::string::npos) {
                if (file != "." && file != "..") {
                    if (getExtension(file) == ext || ext.empty())
                        list.push_back(joinPath(folder, file));
                }
            }
            else if (recursive) {
                const std::vector<std::string> files =
                    listDirectory(joinPath(folder, file), ext, true);
                list.insert(list.end(), files.begin(), files.end());
            }
        }
    }

    return list;
}

std::vector<std::string> Util::listDirectoryFolders(const std::string& path) {
    if (path.empty()) return {};

    DIR* cd;
    struct dirent* cfile;
    std::vector<std::string> list;
    std::string folder = path;
    if (folder[folder.size() - 1] != '/' && folder[folder.size() - 1] != '\\')
        folder.push_back('/');

    cd = opendir(folder.c_str());
    if (cd != nullptr) {
        while ((cfile = readdir(cd))) {
            std::string file = cfile->d_name;
            if (file.find(".") == std::string::npos) { list.push_back(file); }
        }
    }

    return list;
}

bool Util::deleteFile(const std::string& file) { return 0 == remove(file.c_str()); }

} // namespace file
} // namespace bl
