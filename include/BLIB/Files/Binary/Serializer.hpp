#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#define BLIB_FILES_BINARY_SERIALIZER_HPP

#include <BLIB/Files/Binary/BinaryFile.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

#include <type_traits>

namespace bl
{
namespace bf
{
/**
 * @brief Helper struct for serializing types into binary files. Integral types, strings, vector,
 *        and SerializableObjects are supported out of the box. Specializations may be provided for
 *        user defined types
 *
 * @tparam T The type to serialize
 * @tparam bool Helper for integral specialization. Can be ignored
 * @ingroup BinaryFiles
 */
template<typename T, bool = std::is_integral<T>::value>
struct Serializer {
    /**
     * @brief Serialize the given value to the given file
     *
     * @param output The file to write to
     * @param value The value to write
     * @return True if written, false on error
     */
    static bool serialize(BinaryFile& output, const T& value);

    /**
     * @brief Reads the value from input into result
     *
     * @param input The file to read from
     * @param result The object to read into
     * @return True if read, false on error
     */
    static bool deserialize(BinaryFile& input, T& result);

    /**
     * @brief Returns the size of the value when serialized
     *
     * @param value The value to be serialized
     * @return std::uint32_t The size of the value when serialized
     */
    static std::uint32_t size(const T& value);
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
struct Serializer<T, false> {
    static bool serialize(BinaryFile& output, const T& value) { return value.serialize(output); }

    static bool deserialize(BinaryFile& input, T& result) { return result.deserialize(input); }

    static std::uint32_t size(const T& value) { return value.size(); }
};

template<typename T>
struct Serializer<T, true> {
    static bool serialize(BinaryFile& output, const T& value) { return output.write<T>(value); }

    static bool deserialize(BinaryFile& input, T& result) { return input.read<T>(result); }

    static std::uint32_t size(const T&) { return sizeof(T); }
};

template<>
struct Serializer<std::string> {
    static bool serialize(BinaryFile& output, const std::string& value) {
        return output.write(value);
    }

    static bool deserialize(BinaryFile& input, std::string& result) { return input.read(result); }

    static std::uint32_t size(const std::string& s) { return sizeof(std::uint32_t) + s.size(); }
};

template<typename U>
struct Serializer<std::vector<U>, false> {
    static bool serialize(BinaryFile& output, const std::vector<U>& value) {
        if (!output.write<std::uint32_t>(value.size())) return false;
        for (const U& v : value) {
            if (!Serializer<U>::serialize(output, v)) return false;
        }
        return true;
    }

    static bool deserialize(BinaryFile& input, std::vector<U>& value) {
        value.clear();
        std::uint32_t size;
        if (!input.read<std::uint32_t>(size)) return false;
        value.resize(size);
        for (unsigned int i = 0; i < size; ++i) {
            if (!Serializer<U>::deserialize(input, value[i])) return false;
        }
        return true;
    }

    static std::uint32_t size(const std::vector<U>& v) {
        std::uint32_t vs = 0;
        for (const U& u : v) { vs += Serializer<U>::size(u); }
        return sizeof(std::uint32_t) + vs;
    }
};

} // namespace bf
} // namespace bl

#endif
