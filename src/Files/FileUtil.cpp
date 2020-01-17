#include <BLIB/Files/FileUtil.hpp>

#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

namespace bl
{
bool FileUtil::exists(const std::string& file) {
    struct stat buffer;
    return (stat(file.c_str(), &buffer) == 0);
}

bool FileUtil::isBigEndian() {
    union {
        uint32_t i;
        char c[4];
    } endTest = {0x01020304};
    return endTest.c[0] == 1;
}

std::string FileUtil::getExtension(const std::string& file) {
    size_t i = file.find_last_of(".");
    if (i != std::string::npos) return file.substr(i + 1);
    i = file.find_last_of("/\\");
    if (i != std::string::npos) return file.substr(i + 1);
    return file;
}

std::string FileUtil::getBaseName(const std::string& file) {
    std::string base = getFilename(file);
    size_t i         = base.find_last_of(".");
    if (i != std::string::npos) base.erase(i);
    return base;
}

std::string FileUtil::getFilename(const std::string& file) {
    const size_t i = file.find_last_of("/\\");
    if (i != std::string::npos) return file.substr(i + 1);
    return file;
}

std::string FileUtil::getPath(const std::string& file) {
    const size_t i = file.find_last_of("/\\");
    return file.substr(0, (i != std::string::npos) ? (i + 1) : (i));
}

std::string FileUtil::joinPath(const std::string& l, const std::string& r) {
    size_t ls = std::string::npos;
    if (l.size() > 0) {
        if (l[l.size() - 1] == '/' || l[l.size() - 1] == '\\') ls = l.size() - 1;
    }

    size_t rs = 0;
    if (r.size() > 0) {
        if (r[0] == '/' || r[0] == '\\') rs = 1;
    }

    return l.substr(0, ls) + "/" + r.substr(rs);
}

void FileUtil::copyFile(const std::string& src, const std::string& dest) {
    if (src == dest) return;

    std::ifstream source(src.c_str(), std::ios::binary);
    std::ofstream dst(dest.c_str(), std::ios::binary);

    std::istreambuf_iterator<char> begin_source(source);
    std::istreambuf_iterator<char> end_source;
    std::ostreambuf_iterator<char> begin_dest(dst);
    std::copy(begin_source, end_source, begin_dest);
}

bool FileUtil::createDirectory(const std::string& path) {
    std::string cd;
    cd.reserve(path.size());
    for (unsigned int i = 0; i < path.size(); ++i) {
        if (path[i] == '/' || path[i] == '\\') {
            if (mkdir(cd.c_str(), 0775) != 0) return false;
        }
    }
    return true;
}

std::vector<std::string> FileUtil::listDirectory(
    const std::string& path, const std::string& ext, bool recursive) {
    if (path.empty()) return {};

    DIR* cd;
    struct dirent* cfile;
    std::vector<std::string> list;
    std::string folder = path;
    if (folder[folder.size() - 1] != '/' && folder[folder.size() - 1] != '\\')
        folder.push_back('/');

    cd = opendir(folder.c_str());
    while (cd != nullptr) {
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

} // namespace bl