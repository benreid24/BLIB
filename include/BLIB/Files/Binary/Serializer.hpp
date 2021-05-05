#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#define BLIB_FILES_BINARY_SERIALIZER_HPP

#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Files/Binary/File.hpp>
#include <BLIB/Files/Binary/SerializableObject.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <type_traits>

namespace bl
{
namespace file
{
namespace binary
{
/**
 * @brief Helper struct for serializing types into binary files. Integral types, strings, vector,
 *        and SerializableObjects are supported out of the box. Specializations may be provided for
 *        user defined types
 *
 * @tparam T The type to serialize
 * @tparam bool Helper for integral specialization. Can be ignored
 * @ingroup Binary
 */
template<typename T, bool = std::is_integral<T>::value || std::is_enum<T>::value>
struct Serializer {
    /**
     * @brief Serialize the given value to the given file
     *
     * @param output The file to write to
     * @param value The value to write
     * @return True if written, false on error
     */
    static bool serialize(File& output, const T& value);

    /**
     * @brief Reads the value from input into result
     *
     * @param input The file to read from
     * @param result The object to read into
     * @return True if read, false on error
     */
    static bool deserialize(File& input, T& result);

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
    static bool serialize(File& output, const T& value) { return value.serialize(output); }

    static bool deserialize(File& input, T& result) { return result.deserialize(input); }

    static std::uint32_t size(const T& value) { return value.size(); }
};

template<typename T>
struct Serializer<T, true> {
    static bool serialize(File& output, const T& value) {
        if constexpr (std::is_enum<T>::value) {
            return output.write<std::underlying_type_t<T>>(
                static_cast<std::underlying_type_t<T>>(value));
        }
        else {
            return output.write<T>(value);
        }
    }

    static bool deserialize(File& input, T& result) {
        if constexpr (std::is_enum<T>::value) {
            return input.read<std::underlying_type_t<T>>(
                *reinterpret_cast<std::underlying_type_t<T>*>(&result));
        }
        else {
            return input.read<T>(result);
        }
    }

    static std::uint32_t size(const T&) { return sizeof(T); }
};

template<>
struct Serializer<std::string> {
    static bool serialize(File& output, const std::string& value) { return output.write(value); }

    static bool deserialize(File& input, std::string& result) { return input.read(result); }

    static std::uint32_t size(const std::string& s) { return sizeof(std::uint32_t) + s.size(); }
};

template<typename U>
struct Serializer<std::vector<U>, false> {
    static bool serialize(File& output, const std::vector<U>& value) {
        if (!output.write<std::uint32_t>(value.size())) return false;
        for (const U& v : value) {
            if (!Serializer<U>::serialize(output, v)) return false;
        }
        return true;
    }

    static bool deserialize(File& input, std::vector<U>& value) {
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

template<typename U>
struct Serializer<container::Vector2D<U>, false> {
    static bool serialize(File& output, const container::Vector2D<U>& val) {
        if (!output.write<std::uint32_t>(val.getWidth())) return false;
        if (!output.write<std::uint32_t>(val.getHeight())) return false;
        for (unsigned int x = 0; x < val.getWidth(); ++x) {
            for (unsigned int y = 0; y < val.getHeight(); ++y) {
                if (!Serializer<U>::serialize(output, val(x, y))) return false;
            }
        }
        return true;
    }

    static bool deserialize(File& input, container::Vector2D<U>& result) {
        std::uint32_t w, h;
        if (!input.read<std::uint32_t>(w)) return false;
        if (!input.read<std::uint32_t>(h)) return false;
        result.setSize(w, h);
        for (unsigned int x = 0; x < w; ++x) {
            for (unsigned int y = 0; y < h; ++y) {
                if (!Serializer<U>::deserialize(input, result(x, y))) return false;
            }
        }
        return true;
    }

    static std::uint32_t size(const container::Vector2D<U>& val) {
        std::uint32_t s = sizeof(std::uint32_t) * 2;
        for (unsigned int x = 0; x < val.getWidth(); ++x) {
            for (unsigned int y = 0; y < val.getHeight(); ++y) {
                s += Serializer<U>::size(val(x, y));
            }
        }
        return s;
    }
};

template<typename TKey, typename TValue>
struct Serializer<std::unordered_map<TKey, TValue>, false> {
    static bool serialize(File& output, const std::unordered_map<TKey, TValue>& value) {
        if (!output.write<std::uint32_t>(value.size())) return false;
        for (const auto& pair : value) {
            if (!Serializer<TKey>::serialize(output, pair.first)) return false;
            if (!Serializer<TValue>::serialize(output, pair.second)) return false;
        }
        return true;
    }

    static bool deserialize(File& input, std::unordered_map<TKey, TValue>& value) {
        std::uint32_t size;
        if (!input.read<std::uint32_t>(size)) return false;
        for (std::uint32_t i = 0; i < size; ++i) {
            TKey key;
            if (!Serializer<TKey>::deserialize(input, key)) return false;
            auto it = value.try_emplace(key).first;
            if (!Serializer<TValue>::deserialize(input, it->second)) return false;
        }
        return true;
    }

    static std::uint32_t size(const std::unordered_map<TKey, TValue>& value) {
        std::uint32_t ms = 0;
        for (const auto& pair : value) {
            ms += Serializer<TKey>::size(pair.first);
            ms += Serializer<TValue>::size(pair.second);
        }
        return sizeof(std::uint32_t) + ms;
    }
};

template<>
struct Serializer<sf::Vector2i> {
    static bool serialize(File& output, const sf::Vector2i& v) {
        if (!output.write<std::int32_t>(v.x)) return false;
        return output.write<std::int32_t>(v.y);
    }

    static bool deserialize(File& input, sf::Vector2i& v) {
        if (!input.read<std::int32_t>(v.x)) return false;
        return input.read<std::int32_t>(v.y);
    }

    static std::uint32_t size(const sf::Vector2i&) { return sizeof(std::int32_t) * 2; }
};

template<>
struct Serializer<sf::IntRect> {
    static bool serialize(File& output, const sf::IntRect& r) {
        if (!output.write<std::int32_t>(r.left)) return false;
        if (!output.write<std::int32_t>(r.top)) return false;
        if (!output.write<std::int32_t>(r.width)) return false;
        return output.write<std::int32_t>(r.height);
    }

    static bool deserialize(File& input, sf::IntRect& r) {
        if (!input.read<std::int32_t>(r.left)) return false;
        if (!input.read<std::int32_t>(r.top)) return false;
        if (!input.read<std::int32_t>(r.width)) return false;
        return input.read<std::int32_t>(r.height);
    }

    static std::uint32_t size(const sf::IntRect&) { return sizeof(std::int32_t) * 4; }
};

template<>
struct Serializer<sf::Vector2u> {
    static bool serialize(File& output, const sf::Vector2u& v) {
        if (!output.write<std::uint32_t>(v.x)) return false;
        return output.write<std::uint32_t>(v.y);
    }

    static bool deserialize(File& input, sf::Vector2u& v) {
        if (!input.read<std::uint32_t>(v.x)) return false;
        return input.read<std::uint32_t>(v.y);
    }

    static std::uint32_t size(const sf::Vector2u&) { return sizeof(std::uint32_t) * 2; }
};

} // namespace binary
} // namespace file
} // namespace bl

#endif
