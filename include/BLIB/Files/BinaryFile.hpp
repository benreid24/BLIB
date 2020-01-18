#ifndef BLIB_FILES_BINARYFILE_HPP
#define BLIB_FILES_BINARYFILE_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Files/FileUtil.hpp>
#include <fstream>

namespace bl
{

/**
 * Utility class to read and write binary data. Endianness agnostic
 * 
 * \ingroup Files
 */
class BinaryFile : private NonCopyable {
public:
    enum OpenMode {
        Read,
        Write
    };

    BinaryFile(const std::string& path, OpenMode mode = Read);
    ~BinaryFile();

    template<typename T>
    bool write(const T& data);

    template<typename T>
    bool read(T& output);

    bool good() const;

private:
    const OpenMode mode;
    std::fstream handle;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

BinaryFile::BinaryFile(const std::string& path, OpenMode mode) : mode(mode) {
    if (mode == Read)
        handle.open(path.c_str(), std::ios::in | std::ios::binary);
    else
        handle.open(path.c_str(), std::ios::out | std::ios::binary);
}

BinaryFile::~BinaryFile() {
    handle.close();
}

template<typename T>
bool BinaryFile::write(const T& data) {
    if (!handle.good() || mode!=Write)
        return false;

    const size_t size = sizeof(T);
    char bytes[size];
    for (unsigned int i = 0; i<size; ++i) {
        const unsigned int j = FileUtil::isBigEndian() ? i : (size - i - 1);
        bytes[i] = static_cast<uint8_t>(data >> (j*8));
    }
    handle.write(bytes, size);
    return handle.good();
}

template<>
bool BinaryFile::write<std::string>(const std::string& data) {
    if (!handle.good() || mode!=Write)
        return false;
    if (!write<uint32_t>(data.size()))
        return false;
    handle.write(data.c_str(), data.size());
    return handle.good();
}

template<typename T>
bool BinaryFile::read(T& output) {
    if (!handle.good() || mode!=Read)
        return false;
    
    const size_t size = sizeof(output);
    char bytes[size];
    
    output = 0;
    handle.read(static_cast<char*>(bytes), size);
    for (unsigned int i = 0; i<size; ++i) {
        const unsigned int j = FileUtil::isBigEndian() ? i : (size - i - 1);
        output |= static_cast<T>(bytes[i] << (j*8));
    }
    return handle.good();
}

template<>
bool BinaryFile::read<std::string>(std::string& output) {
    if (!handle.good() || mode != Read)
        return false;
    uint32_t size;
    if (!read<uint32_t>(size))
        return false;
    char* buf = new char[size];
    handle.read(buf, size);
    output = std::string(buf, size);
    return handle.good();
}

inline bool BinaryFile::good() const {
    return handle.good();
}

}

#endif