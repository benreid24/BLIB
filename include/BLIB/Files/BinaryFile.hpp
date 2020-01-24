#ifndef BLIB_FILES_BINARYFILE_HPP
#define BLIB_FILES_BINARYFILE_HPP

#include <BLIB/Files/FileUtil.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <cstring>
#include <fstream>
#include <type_traits>

namespace bl
{
/**
 * @brief Utility class to read and write binary data. Endianness agnostic
 *
 * @ingroup Files
 */
class BinaryFile : private NonCopyable {
public:
    enum OpenMode { Read, Write };

    BinaryFile(const std::string& path, OpenMode mode = Read);
    ~BinaryFile();

    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, bool>::type write(const T& data);
    bool write(const std::string& data);

    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, bool>::type read(T& output);
    bool read(std::string& output);

    bool good();

private:
    const OpenMode mode;
    std::fstream handle;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

BinaryFile::BinaryFile(const std::string& path, OpenMode mode)
: mode(mode) {
    if (mode == Read)
        handle.open(path.c_str(), std::ios::in | std::ios::binary);
    else
        handle.open(path.c_str(), std::ios::out | std::ios::binary);
}

BinaryFile::~BinaryFile() {
    handle.close();
}

template<typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type BinaryFile::write(const T& data) {
    if (!handle.good() || mode != Write) return false;

    const size_t size = sizeof(T);
    char bytes[size];
    std::memcpy(bytes, &data, size);
    if (FileUtil::isBigEndian()) {
        for (unsigned int i = 0; i < size / 2; ++i) { std::swap(bytes[i], bytes[size - i - 1]); }
    }
    handle.write(bytes, size);
    return handle.good();
}

bool BinaryFile::write(const std::string& data) {
    if (!handle.good() || mode != Write) return false;
    if (!write<uint32_t>(data.size())) return false;
    handle.write(data.c_str(), data.size());
    return handle.good();
}

template<typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type BinaryFile::read(T& output) {
    if (!handle.good() || mode != Read) return false;

    const size_t size = sizeof(output);
    char bytes[size];

    output = 0;
    handle.read(bytes, size);
    if (FileUtil::isBigEndian()) {
        for (unsigned int i = 0; i < size / 2; ++i) { std::swap(bytes[i], bytes[size - i - 1]); }
    }
    std::memcpy(&output, bytes, size);
    return handle.good();
}

bool BinaryFile::read(std::string& output) {
    if (!handle.good() || mode != Read) return false;
    uint32_t size;
    if (!read<uint32_t>(size)) return false;
    char* buf = new char[size];
    handle.read(buf, size);
    output = std::string(buf, size);
    return handle.good();
}

inline bool BinaryFile::good() {
    const bool eof = mode == Read ? handle.peek() == EOF : false;
    return handle.good() && !eof;
}

} // namespace bl

#endif