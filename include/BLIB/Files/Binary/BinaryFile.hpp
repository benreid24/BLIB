#ifndef BLIB_FILES_BINARY_BINARYFILE_HPP
#define BLIB_FILES_BINARY_BINARYFILE_HPP

#include <BLIB/Files/FileUtil.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <cstring>
#include <fstream>
#include <type_traits>

namespace bl
{
/**
 * @brief Utility class to read and write binary data. Endianness agnostic
 *        Floating point types are not supported. To store floating point
 *        values, simply store an integral type and multiply/divide on
 *        save/load
 *
 * @ingroup Files
 */
class BinaryFile : private NonCopyable {
public:
    /**
     * @brief Indicates file operation mode
     *
     */
    enum OpenMode { Read, Write };

    /**
     * @brief Creates a handle to the given file in the given mode
     *
     */
    BinaryFile(const std::string& path, OpenMode mode = Read);

    /**
     * @brief Closes the file handle
     *
     */
    ~BinaryFile();

    /**
     * @brief Writes an integral type to the file in it's binary representation
     *
     * @tparam T The type to write. It is better to be explicit for code to be cross platform
     * @param data The value to write
     * @return bool True if the value could be written
     */
    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, bool>::type write(const T& data);

    /**
     * @brief Writes the string to the file. Stores 4 bytes for the length, then the size of
     * the string
     *
     * @param data The string to write
     * @return bool True if the value could be written
     */
    bool write(const std::string& data);

    /**
     * @brief Reads the integral type from the file
     *
     * @tparam T The type to read. Be explicit with bit width and sign
     * @param output Variable to write the read value to
     * @return bool True if the value could be read
     */
    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, bool>::type read(T& output);

    /**
     * @brief Reads a string from the file
     *
     * @param output Variable to write the read string to
     * @return bool True if the string could be read
     */
    bool read(std::string& output);

    /**
     * @brief Returns the status of the file handle
     *
     * @return bool Returns false if EOF is reached or the file did not exist
     */
    bool good();

private:
    const OpenMode mode;
    std::fstream handle;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

inline BinaryFile::BinaryFile(const std::string& path, OpenMode mode)
: mode(mode) {
    if (mode == Read)
        handle.open(path.c_str(), std::ios::in | std::ios::binary);
    else
        handle.open(path.c_str(), std::ios::out | std::ios::binary);
}

inline BinaryFile::~BinaryFile() { handle.close(); }

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

inline bool BinaryFile::write(const std::string& data) {
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

inline bool BinaryFile::read(std::string& output) {
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