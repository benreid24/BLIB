#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#define BLIB_FILES_BINARY_SERIALIZER_HPP

#include <BLIB/Files/Binary/BinaryFile.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

namespace bl
{
namespace bf
{
template<typename T>
struct Serializer {
    static bool serialize(BinaryFile& output, const T& value);

    static bool deserialize(BinaryFile& input, T& result);
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
bool Serializer<T>::serialize(BinaryFile& output, const T& value) {
    return output.write<T>(value);
}

template<typename T>
bool Serializer<T>::deserialize(BinaryFile& input, T& result) {
    return input.read<T>(result);
}

template<>
struct Serializer<std::string> {
    static bool serialize(BinaryFile& output, const std::string& value) { return output.write(value); }

    static bool deserialize(BinaryFile& input, std::string& result) { return input.read(result); }
};

template<typename U>
struct Serializer<std::vector<U>> {
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
};

template<>
struct Serializer<SerializableObject> {
    static bool serialize(BinaryFile& output, const SerializableObject& value) {
        return value.serialize(output);
    }

    static bool deserialize(BinaryFile& input, SerializableObject& value) {
        return value.deserialize(input);
    }
};

} // namespace bf
} // namespace bl

#endif
