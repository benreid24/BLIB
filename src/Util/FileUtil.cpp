#ifdef BLIB_WINDOWS
#define NOMINMAX
#endif

#include <BLIB/Util/FileUtil.hpp>

#include <BLIB/Util/Random.hpp>
#include <filesystem>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stack>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef BLIB_WINDOWS
#include "dirent_windows.h"
#include <direct.h>
#include <shlobj.h>
#else
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#endif

namespace bl
{
namespace util
{
bool FileUtil::exists(const std::string& file) { return std::filesystem::is_regular_file(file); }

bool FileUtil::directoryExists(const std::string& file) {
    return std::filesystem::is_directory(file);
}

bool FileUtil::isBigEndian() {
    union {
        std::uint32_t i;
        char c[4];
    } endTest = {0x01020304};
    return endTest.c[0] == 1;
}

std::string FileUtil::getExtension(const std::string& file) {
    std::string ext = std::filesystem::path(file).extension();
    if (ext.size() >= 1 && ext[0] == '.') { ext.erase(0, 1); }
    return ext;
}

std::string FileUtil::getBaseName(const std::string& file) {
    return std::filesystem::path(file).stem();
}

std::string FileUtil::getFilename(const std::string& file) {
    return std::filesystem::path(file).filename();
}

std::string FileUtil::getPath(const std::string& file) {
    return std::filesystem::path(file).parent_path();
}

std::string FileUtil::joinPath(const std::string& l, const std::string& r) {
    if (l.empty()) return r;
    if (r.empty()) return l;

    const bool rabs = r[0] == '/' || r[0] == '\\';
    const std::string_view rv(rabs ? r.c_str() + 1 : r.c_str(), rabs ? r.size() - 1 : r.size());
    auto result = std::filesystem::path(l);
    result /= std::filesystem::path(rv);
    return result;
}

bool FileUtil::startsWithPath(const std::string& file, const std::string& path) {
    const std::string_view fileStart(file.c_str(), std::min(file.size(), path.size()));
    return fileStart == path;
}

std::string FileUtil::genTempName(const std::string& path, const std::string& ext) {
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

void FileUtil::copyFile(const std::string& src, const std::string& dest) {
    if (src == dest) return;
    std::filesystem::copy_file(src, dest);
}

bool FileUtil::createDirectory(const std::string& path) {
    return std::filesystem::create_directories(path);
}

std::vector<std::string> FileUtil::listDirectory(const std::string& path, const std::string& ext,
                                                 bool recursive) {
    if (path.empty()) return {};

    std::vector<std::string> list;

    const auto append = [&list, &ext, &path](const std::filesystem::path& cpath) {
        bool add = true;
        if (!ext.empty()) {
            const auto e = cpath.extension();
            const std::string_view es(e.empty() ? e.c_str() : e.c_str() + 1,
                                      e.empty() ? 0 : std::distance(e.begin(), e.end()) - 1);
            add = ext == es;
        }
        if (add) { list.emplace_back(std::filesystem::relative(cpath, path)); }
    };

    if (recursive) {
        for (const auto& cpath : std::filesystem::recursive_directory_iterator(path)) {
            append(cpath);
        }
    }
    else {
        for (const auto& cpath : std::filesystem::directory_iterator(path)) { append(cpath); }
    }

    return list;
}

std::vector<std::string> FileUtil::listDirectoryFolders(const std::string& path) {
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
            const std::string file = cfile->d_name;
            const std::string full = joinPath(folder, file);
            if (file != "." && file != ".." && directoryExists(full)) { list.push_back(file); }
        }
    }

    return list;
}

bool FileUtil::deleteFile(const std::string& file) { return 0 == remove(file.c_str()); }

bool FileUtil::deleteDirectory(const std::string& path) {
    const std::vector<std::string> folders = listDirectoryFolders(path);
    for (const std::string& d : folders) {
        if (!deleteDirectory(joinPath(path, d))) { return false; }
    }

    const std::vector<std::string> files = listDirectory(path);
    for (const std::string& file : files) {
        if (!deleteFile(file)) { return false; }
    }

#ifdef BLIB_WINDOWS
    return 0 == _rmdir(path.c_str());
#else
    return 0 == rmdir(path.c_str());
#endif
}

std::string FileUtil::getDataDirectory(const std::string& appName) {
    std::string path;
#ifdef BLIB_WINDOWS
    char buf[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, 0, 0, buf);
    path = joinPath(buf, "My Games");
    createDirectory(path);
#else
    struct passwd* pwd = getpwuid(getuid());
    path               = pwd ? pwd->pw_dir : "";
#endif

    path = joinPath(path, appName);
    createDirectory(path);
    return path;
}

bool FileUtil::readFile(const std::string& filename, std::string& out) {
    std::ifstream file(filename.c_str());
    if (!file.good()) return false;

    file.seekg(0, std::ios::end);
    out.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(out.data(), out.size());
    return true;
}

bool FileUtil::readFile(const std::string& filename, std::vector<char>& out) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.good()) return false;

    file.seekg(0, std::ios::end);
    out.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(out.data(), out.size());
    return true;
}

bool FileUtil::queryFileInfo(const std::string& path, FileInfo& result) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) return false;
    result.creationTime = info.st_ctime;
    result.modifiedTime = info.st_mtime;
    result.size         = info.st_size;
    return true;
}

} // namespace util
} // namespace bl
