#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#define BLIB_FILES_BINARY_SERIALIZER_HPP

#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Reflection/Attributes/DefaultValue.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Scripts/Value.hpp>
#include <BLIB/Serialization/Binary/Detail/InputStreamWrapper.hpp>
#include <BLIB/Serialization/Binary/Detail/OutputStreamWrapper.hpp>
#include <BLIB/Serialization/Traits.hpp>
#include <BLIB/Streams.hpp>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>
#include <cstring>
#include <glm/glm.hpp>
#include <optional>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace bl
{
namespace serial
{
namespace binary
{
/**
 * @brief Helper struct for serializing types into binary streams. Integral types, strings, vector,
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
     */
    static void serialize(stream::OutputStream& output, const T& value);

    /**
     * @brief Reads the value from input into result
     *
     * @param input The file to read from
     * @param result The object to read into
     */
    static void deserialize(stream::InputStream& input, T& result);

    /**
     * @brief Returns the size of the value when serialized
     *
     * @param value The value to be serialized
     * @return std::uint32_t The size of the value when serialized
     */
    static std::uint32_t size(const T& value);
};

/**
 * @brief Checked interface for Serializer that catches exceptions and returns status instead
 *
 * @tparam T The type to serialize or deserialize
 * @ingroup Binary
 */
template<typename T>
struct SerializerChecked {
    /**
     * @brief Serialize the given value to the given file
     *
     * @param output The file to write to
     * @param value The value to write
     * @return True on success, false on error
     */
    static bool serialize(stream::OutputStream& output, const T& value) {
        try {
            Serializer<T>::serialize(output, value);
            return true;
        } catch (const stream::StreamError&) { return false; }
    }

    /**
     * @brief Reads the value from input into result
     *
     * @param input The file to read from
     * @param result The object to read into
     * @return True on success, false on error
     */
    static bool deserialize(stream::InputStream& input, T& result) {
        try {
            Serializer<T>::deserialize(input, result);
            return true;
        } catch (const stream::StreamError&) { return false; }
    }

    /**
     * @brief Returns the size of the value when serialized
     *
     * @param value The value to be serialized
     * @return std::uint32_t The size of the value when serialized
     */
    static std::uint32_t size(const T& value) { return Serializer<T>::size(value); }
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

#include <BLIB/Serialization/Binary/Detail/DeserializePackedVisitor.inl>
#include <BLIB/Serialization/Binary/Detail/DeserializeVisitor.inl>
#include <BLIB/Serialization/Binary/Detail/SerializeVisitor.inl>
#include <BLIB/Serialization/Binary/Detail/SizeVisitor.inl>

template<typename T>
struct Serializer<T, false> {
    static void serialize(stream::OutputStream& output, const T& value) {
        try {
            detail::serializeVisitor(output, value, false);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to serialize type ") + refl::getNameSafe<T>());
        }
    }

    static void deserialize(stream::InputStream& input, T& result) {
        try {
            detail::deserializeVisitor(input, result);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to deserialize type ") + refl::getNameSafe<T>());
        }
    }

    static void serializePacked(stream::OutputStream& output, const T& value) {
        try {
            detail::serializeVisitor(output, value, true);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to serialize type ") + refl::getNameSafe<T>());
        }
    }

    static void deserializePacked(stream::InputStream& input, T& result) {
        try {
            detail::deserializePackedVisitor(input, result);
        } catch (const SerializationError& e) {
            throw SerializationError(
                e, std::string("Failed to deserialize type ") + refl::getNameSafe<T>());
        }
    }

    static std::uint32_t size(const T& value) { return detail::sizeVisitor(value); }
};

template<typename T>
struct Serializer<T, true> {
    static void serialize(stream::OutputStream& output, const T& value) {
        detail::OutputStreamWrapper wrapper(output);
        if constexpr (std::is_enum<T>::value) {
            wrapper.write<std::underlying_type_t<T>>(static_cast<std::underlying_type_t<T>>(value));
        }
        else if constexpr (std::is_same_v<T, bool>) { wrapper.write<std::uint8_t>(value); }
        else { wrapper.write<T>(value); }
    }

    static void deserialize(stream::InputStream& input, T& result) {
        detail::InputStreamWrapper wrapper(input);
        if constexpr (std::is_enum<T>::value) {
            wrapper.read<std::underlying_type_t<T>>(
                *reinterpret_cast<std::underlying_type_t<T>*>(&result));
        }
        else if constexpr (std::is_same_v<T, bool>) {
            std::uint8_t v;
            wrapper.read<std::uint8_t>(v);
            result = v != 0;
        }
        else { wrapper.read<T>(result); }
    }

    static std::uint32_t size(const T&) { return sizeof(T); }
};

template<>
struct Serializer<bool, false> {
    static void serialize(stream::OutputStream& output, const bool& value) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::uint8_t>(value ? 1 : 0);
    }

    static void deserialize(stream::InputStream& input, bool& value) {
        detail::InputStreamWrapper wrapper(input);
        std::uint8_t v;
        wrapper.read<std::uint8_t>(v);
        value = v == 1;
    }

    static std::uint32_t size(const bool&) { return sizeof(std::uint8_t); }
};

template<>
struct Serializer<std::string> {
    static void serialize(stream::OutputStream& output, const std::string& value) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write(value);
    }

    static void deserialize(stream::InputStream& input, std::string& result) {
        detail::InputStreamWrapper wrapper(input);
        wrapper.read(result);
    }

    static std::uint32_t size(const std::string& s) {
        return static_cast<std::uint32_t>(sizeof(std::uint32_t) + s.size());
    }
};

template<>
struct Serializer<float> {
    static void serialize(stream::OutputStream& output, const float& value) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write(std::to_string(value));
    }

    static void deserialize(stream::InputStream& input, float& result) {
        detail::InputStreamWrapper wrapper(input);
        std::string str;
        wrapper.read(str);
        try {
            result = std::stof(str);
        } catch (...) { throw SerializationError("Failed to parse float from serialized string"); }
    }

    static std::uint32_t size(const float& s) {
        return Serializer<std::string>::size(std::to_string(s));
    }
};

template<>
struct Serializer<double> {
    static void serialize(stream::OutputStream& output, const double& value) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write(std::to_string(value));
    }

    static void deserialize(stream::InputStream& input, double& result) {
        detail::InputStreamWrapper wrapper(input);
        std::string str;
        wrapper.read(str);
        try {
            result = std::stod(str);
        } catch (...) { throw SerializationError("Failed to parse double from serialized string"); }
    }

    static std::uint32_t size(const double& s) {
        return Serializer<std::string>::size(std::to_string(s));
    }
};

template<typename U, std::size_t N>
struct Serializer<U[N], false> {
    static void serialize(stream::OutputStream& out, const U* arr) {
        if (!arr) { throw SerializationError("Cannot serialize null fixed-size array"); }
        detail::OutputStreamWrapper wrapper(out);
        wrapper.write<std::uint32_t>(N);
        for (std::size_t i = 0; i < N; ++i) { Serializer<U>::serialize(out, arr[i]); }
    }

    static void deserialize(stream::InputStream& in, U* arr) {
        if (!arr) { throw SerializationError("Cannot deserialize into null fixed-size array"); }
        detail::InputStreamWrapper wrapper(in);
        std::uint32_t n = 0;
        wrapper.read<std::uint32_t>(n);
        if (n != N) { throw SerializationError("Serialized fixed-size array length mismatch"); }
        for (std::size_t i = 0; i < N; ++i) { Serializer<U>::deserialize(in, arr[i]); }
    }

    static std::size_t size(const U* arr) {
        std::size_t s = sizeof(std::uint32_t);
        for (std::size_t i = 0; i < N; ++i) { s += Serializer<U>::size(arr[i]); }
        return s;
    }
};

template<typename U, std::size_t N>
struct Serializer<std::array<U, N>, false> {
    using T = std::array<U, N>;

    static void serialize(stream::OutputStream& out, const T& arr) {
        detail::OutputStreamWrapper wrapper(out);
        wrapper.write<std::uint32_t>(N);
        for (std::size_t i = 0; i < N; ++i) { Serializer<U>::serialize(out, arr[i]); }
    }

    static void deserialize(stream::InputStream& in, T& arr) {
        detail::InputStreamWrapper wrapper(in);
        std::uint32_t n = 0;
        wrapper.read<std::uint32_t>(n);
        if (n != N) { throw SerializationError("Serialized std::array length mismatch"); }
        for (std::size_t i = 0; i < N; ++i) { Serializer<U>::deserialize(in, arr[i]); }
    }

    static std::size_t size(const T& arr) {
        std::size_t s = sizeof(std::uint32_t);
        for (std::size_t i = 0; i < N; ++i) { s += Serializer<U>::size(arr[i]); }
        return s;
    }
};

template<typename U>
struct Serializer<std::vector<U>, false> {
    static void serialize(stream::OutputStream& output, const std::vector<U>& value) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::uint32_t>(value.size());
        for (const U& v : value) { Serializer<U>::serialize(output, v); }
    }

    static void deserialize(stream::InputStream& input, std::vector<U>& value) {
        detail::InputStreamWrapper wrapper(input);
        value.clear();
        std::uint32_t size;
        wrapper.read<std::uint32_t>(size);
        value.resize(size);
        for (unsigned int i = 0; i < size; ++i) { Serializer<U>::deserialize(input, value[i]); }
    }

    static std::uint32_t size(const std::vector<U>& v) {
        std::uint32_t vs = 0;
        for (const U& u : v) { vs += Serializer<U>::size(u); }
        return sizeof(std::uint32_t) + vs;
    }
};

template<typename U>
struct Serializer<ctr::Vector2D<U>, false> {
    static void serialize(stream::OutputStream& output, const ctr::Vector2D<U>& val) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::uint32_t>(val.getWidth());
        wrapper.write<std::uint32_t>(val.getHeight());
        for (unsigned int x = 0; x < val.getWidth(); ++x) {
            for (unsigned int y = 0; y < val.getHeight(); ++y) {
                Serializer<U>::serialize(output, val(x, y));
            }
        }
    }

    static void deserialize(stream::InputStream& input, ctr::Vector2D<U>& result) {
        detail::InputStreamWrapper wrapper(input);
        std::uint32_t w, h;
        wrapper.read<std::uint32_t>(w);
        wrapper.read<std::uint32_t>(h);
        result.setSize(w, h);
        for (unsigned int x = 0; x < w; ++x) {
            for (unsigned int y = 0; y < h; ++y) {
                Serializer<U>::deserialize(input, result(x, y));
            }
        }
    }

    static std::uint32_t size(const ctr::Vector2D<U>& val) {
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
    static void serialize(stream::OutputStream& output,
                          const std::unordered_map<TKey, TValue>& value) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::uint32_t>(value.size());
        for (const auto& pair : value) {
            Serializer<TKey>::serialize(output, pair.first);
            Serializer<TValue>::serialize(output, pair.second);
        }
    }

    static void deserialize(stream::InputStream& input, std::unordered_map<TKey, TValue>& value) {
        detail::InputStreamWrapper wrapper(input);
        value.clear();
        std::uint32_t size;
        wrapper.read<std::uint32_t>(size);
        for (std::uint32_t i = 0; i < size; ++i) {
            TKey key;
            Serializer<TKey>::deserialize(input, key);
            auto it = value.try_emplace(key).first;
            Serializer<TValue>::deserialize(input, it->second);
        }
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

template<typename U>
struct Serializer<std::unordered_set<U>, false> {
    static void serialize(stream::OutputStream& output, const std::unordered_set<U>& value) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::uint32_t>(value.size());
        for (const U& v : value) { Serializer<U>::serialize(output, v); }
    }

    static void deserialize(stream::InputStream& input, std::unordered_set<U>& value) {
        detail::InputStreamWrapper wrapper(input);
        value.clear();
        std::uint32_t size;
        wrapper.read<std::uint32_t>(size);
        for (unsigned int i = 0; i < size; ++i) {
            U k;
            Serializer<U>::deserialize(input, k);
            value.insert(std::move(k));
        }
    }

    static std::uint32_t size(const std::unordered_set<U>& v) {
        std::uint32_t vs = 0;
        for (const U& u : v) { vs += Serializer<U>::size(u); }
        return sizeof(std::uint32_t) + vs;
    }
};

template<typename U>
struct Serializer<sf::Vector2<U>, false> {
    using S = Serializer<U>;
    static void serialize(stream::OutputStream& output, const sf::Vector2<U>& v) {
        S::serialize(output, v.x);
        S::serialize(output, v.y);
    }

    static void deserialize(stream::InputStream& input, sf::Vector2<U>& v) {
        S::deserialize(input, v.x);
        S::deserialize(input, v.y);
    }

    static std::uint32_t size(const sf::Vector2<U>& v) { return S::size(v.x) + S::size(v.y); }
};

template<typename U>
struct Serializer<sf::Vector3<U>, false> {
    using S = Serializer<U>;
    static void serialize(stream::OutputStream& output, const sf::Vector3<U>& v) {
        S::serialize(output, v.x);
        S::serialize(output, v.y);
        S::serialize(output, v.z);
    }

    static void deserialize(stream::InputStream& input, sf::Vector3<U>& v) {
        S::deserialize(input, v.x);
        S::deserialize(input, v.y);
        S::deserialize(input, v.z);
    }

    static std::uint32_t size(const sf::Vector3<U>& v) { return S::size(v.x) * 3; }
};

template<typename U, glm::qualifier P>
struct Serializer<glm::vec<2, U, P>, false> {
    using S = Serializer<U>;
    using V = glm::vec<2, U, P>;

    static void serialize(stream::OutputStream& output, const V& v) {
        S::serialize(output, v.x);
        S::serialize(output, v.y);
    }

    static void deserialize(stream::InputStream& input, V& v) {
        S::deserialize(input, v.x);
        S::deserialize(input, v.y);
    }

    static std::uint32_t size(const V& v) { return S::size(v.x) + S::size(v.y); }
};

template<typename U, glm::qualifier P>
struct Serializer<glm::vec<3, U, P>, false> {
    using S = Serializer<U>;
    using V = glm::vec<3, U, P>;

    static void serialize(stream::OutputStream& output, const V& v) {
        S::serialize(output, v.x);
        S::serialize(output, v.y);
        S::serialize(output, v.z);
    }

    static void deserialize(stream::InputStream& input, V& v) {
        S::deserialize(input, v.x);
        S::deserialize(input, v.y);
        S::deserialize(input, v.z);
    }

    static std::uint32_t size(const V& v) { return S::size(v.x) + S::size(v.y) + S::size(v.z); }
};

template<typename U, glm::qualifier P>
struct Serializer<glm::vec<4, U, P>, false> {
    using S = Serializer<U>;
    using V = glm::vec<4, U, P>;

    static void serialize(stream::OutputStream& output, const V& v) {
        S::serialize(output, v.x);
        S::serialize(output, v.y);
        S::serialize(output, v.z);
        S::serialize(output, v.w);
    }

    static void deserialize(stream::InputStream& input, V& v) {
        S::deserialize(input, v.x);
        S::deserialize(input, v.y);
        S::deserialize(input, v.z);
        S::deserialize(input, v.w);
    }

    static std::uint32_t size(const V& v) {
        return S::size(v.x) + S::size(v.y) + S::size(v.z) + S::size(v.w);
    }
};

template<typename U, glm::qualifier P>
struct Serializer<glm::qua<U, P>, false> {
    using S = Serializer<U>;
    using V = glm::qua<U, P>;

    static void serialize(stream::OutputStream& output, const V& v) {
        S::serialize(output, v.x);
        S::serialize(output, v.y);
        S::serialize(output, v.z);
        S::serialize(output, v.w);
    }

    static void deserialize(stream::InputStream& input, V& v) {
        S::deserialize(input, v.x);
        S::deserialize(input, v.y);
        S::deserialize(input, v.z);
        S::deserialize(input, v.w);
    }

    static std::uint32_t size(const V& v) {
        return S::size(v.x) + S::size(v.y) + S::size(v.z) + S::size(v.w);
    }
};

template<glm::length_t C, glm::length_t R, typename T, enum glm::qualifier Q>
struct Serializer<glm::mat<C, R, T, Q>, false> {
    using S   = Serializer<T>;
    using Mat = glm::mat<C, R, T, Q>;

    static void serialize(stream::OutputStream& output, const Mat& m) {
        for (int c = 0; c < C; ++c) {
            for (int r = 0; r < R; ++r) { S::serialize(output, m[c][r]); }
        }
    }

    static void deserialize(stream::InputStream& input, Mat& m) {
        for (int c = 0; c < C; ++c) {
            for (int r = 0; r < R; ++r) { S::deserialize(input, m[c][r]); }
        }
    }

    static std::uint32_t size(const Mat& m) { return S::size(m[0][0]) * C * R; }
};

template<>
struct Serializer<sf::IntRect> {
    static void serialize(stream::OutputStream& output, const sf::IntRect& r) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::int32_t>(r.position.x);
        wrapper.write<std::int32_t>(r.position.y);
        wrapper.write<std::int32_t>(r.size.x);
        wrapper.write<std::int32_t>(r.size.y);
    }

    static void deserialize(stream::InputStream& input, sf::IntRect& r) {
        detail::InputStreamWrapper wrapper(input);
        wrapper.read<std::int32_t>(r.position.x);
        wrapper.read<std::int32_t>(r.position.y);
        wrapper.read<std::int32_t>(r.size.x);
        wrapper.read<std::int32_t>(r.size.y);
    }

    static std::uint32_t size(const sf::IntRect&) { return sizeof(std::int32_t) * 4; }
};

template<>
struct Serializer<sf::Vector2u> {
    static void serialize(stream::OutputStream& output, const sf::Vector2u& v) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::uint32_t>(v.x);
        wrapper.write<std::uint32_t>(v.y);
    }

    static void deserialize(stream::InputStream& input, sf::Vector2u& v) {
        detail::InputStreamWrapper wrapper(input);
        wrapper.read<std::uint32_t>(v.x);
        wrapper.read<std::uint32_t>(v.y);
    }

    static std::uint32_t size(const sf::Vector2u&) { return sizeof(std::uint32_t) * 2; }
};

template<typename U, typename V>
struct Serializer<std::pair<U, V>, false> {
    static void serialize(stream::OutputStream& output, const std::pair<U, V>& v) {
        Serializer<U>::serialize(output, v.first);
        Serializer<V>::serialize(output, v.second);
    }

    static void deserialize(stream::InputStream& input, std::pair<U, V>& v) {
        Serializer<U>::deserialize(input, v.first);
        Serializer<V>::deserialize(input, v.second);
    }

    static std::uint32_t size(const std::pair<U, V>& v) {
        return Serializer<U>::size(v.first) + Serializer<V>::size(v.second);
    }
};

template<typename... Ts>
struct Serializer<std::variant<Ts...>, false> {
private:
    struct HelperBase {
        virtual void deserialize(stream::InputStream& input, std::variant<Ts...>& v) const = 0;
    };

    template<typename U>
    struct Helper : public HelperBase {
        virtual void deserialize(stream::InputStream& input,
                                 std::variant<Ts...>& v) const override {
            v.template emplace<U>();
            Serializer<U>::deserialize(input, std::get<U>(v));
        }
    };

public:
    static void serialize(stream::OutputStream& output, const std::variant<Ts...>& v) {
        const auto visitor = [&output](const auto& c) {
            Serializer<std::decay_t<decltype(c)>>::serialize(output, c);
        };

        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::uint16_t>(v.index());
        std::visit(visitor, v);
    }

    static void deserialize(stream::InputStream& input, std::variant<Ts...>& v) {
        static const std::tuple<Helper<Ts>...> helperTuple(Helper<Ts>{}...);
        static const HelperBase* helpers[] = {&std::get<Helper<Ts>>(helperTuple)...};

        detail::InputStreamWrapper wrapper(input);
        std::uint16_t i = 0;
        wrapper.read<std::uint16_t>(i);
        if (i >= sizeof...(Ts)) { throw SerializationError("Variant index out of range"); }
        helpers[i]->deserialize(input, v);
    }

    static std::uint32_t size(const std::variant<Ts...>& v) {
        static constexpr auto visitor = [](const auto& c) -> std::uint32_t {
            return Serializer<std::decay_t<decltype(c)>>::size(c);
        };

        return std::visit(visitor, v) + sizeof(std::uint16_t);
    }
};

template<typename U>
struct Serializer<U*, false> {
    using T = U*;

    static void serialize(stream::OutputStream& output, const T v) {
        if (!v) { throw SerializationError("Cannot serialize null pointer"); }
        Serializer<U>::serialize(output, *v);
    }

    static void deserialize(stream::InputStream& input, T v) {
        if (!v) { throw SerializationError("Cannot deserialize into null pointer"); }
        Serializer<U>::deserialize(input, *v);
    }

    static std::uint32_t size(const T v) {
        if (!v) return 0;
        return Serializer<U>::size(*v);
    }
};

template<>
struct Serializer<script::Value, false> {
    static void serialize(stream::OutputStream& output, const script::Value& v) {
        try {
            const script::PrimitiveValue& value = v.value();
            Serializer<script::PrimitiveValue::Type>::serialize(output, value.getType());
            if (!supported(v.value().getType())) return;

            detail::OutputStreamWrapper wrapper(output);

            switch (v.value().getType()) {
            case script::PrimitiveValue::TBool:
                Serializer<bool>::serialize(output, value.getAsBool());
                break;
            case script::PrimitiveValue::TInteger:
                wrapper.write<std::int32_t>(value.getAsInt());
                break;
            case script::PrimitiveValue::TFloat:
                Serializer<float>::serialize(output, value.getAsFloat());
                break;
            case script::PrimitiveValue::TString:
                Serializer<std::string>::serialize(output, value.getAsString());
                break;
            case script::PrimitiveValue::TArray:
                wrapper.write<std::uint32_t>(static_cast<std::uint32_t>(value.getAsArray().size()));
                for (const script::Value& av : value.getAsArray()) { serialize(output, av); }
                break;
            default:
                break;
            }

            wrapper.write<std::uint32_t>(v.allProperties().size());
            for (const auto& prop : v.allProperties()) {
                Serializer<std::string>::serialize(output, prop.first);
                Serializer<script::Value>::serialize(output, prop.second.deref());
            }
        } catch (const SerializationError& e) {
            throw SerializationError(e, "Failed to serialize script::Value");
        }
    }

    static void deserialize(stream::InputStream& input, script::Value& v) {
        try {
            script::PrimitiveValue& value = v.value();
            script::PrimitiveValue::Type type;
            Serializer<script::PrimitiveValue::Type>::deserialize(input, type);
            if (!supported(type)) {
                value = "<UNSUPPORTED TYPE>";
                return;
            }

            detail::InputStreamWrapper wrapper(input);

            switch (type) {
            case script::PrimitiveValue::TBool: {
                bool bv;
                Serializer<bool>::deserialize(input, bv);
                value = bv;
                break;
            }
            case script::PrimitiveValue::TInteger: {
                std::int32_t i;
                wrapper.read<std::int32_t>(i);
                value = i;
                break;
            }
            case script::PrimitiveValue::TFloat: {
                float f;
                Serializer<float>::deserialize(input, f);
                value = f;
                break;
            }
            case script::PrimitiveValue::TString: {
                std::string s;
                Serializer<std::string>::deserialize(input, s);
                value = s;
                break;
            }
            case script::PrimitiveValue::TArray: {
                std::uint32_t s;
                wrapper.read<std::uint32_t>(s);
                value = script::ArrayValue{};
                value.getAsArray().resize(s);
                for (unsigned int i = 0; i < s; ++i) { deserialize(input, value.getAsArray()[i]); }
                break;
            }
            default:
                break;
            }

            std::uint32_t s;
            wrapper.read<std::uint32_t>(s);
            for (unsigned int i = 0; i < s; ++i) {
                std::string key;
                script::Value pv;
                Serializer<std::string>::deserialize(input, key);
                Serializer<script::Value>::deserialize(input, pv);
                v.setProperty(key, {std::move(pv)});
            }
        } catch (const SerializationError& e) {
            throw SerializationError(e, "Failed to deserialize script::Value");
        }
    }

    static std::uint32_t size(const script::Value& v) {
        std::uint32_t s = Serializer<script::PrimitiveValue::Type>::size(v.value().getType());
        if (!supported(v.value().getType())) return s;

        switch (v.value().getType()) {
        case script::PrimitiveValue::TBool:
            s += Serializer<bool>::size(true);
            break;
        case script::PrimitiveValue::TInteger:
            s += sizeof(std::int32_t);
            break;
        case script::PrimitiveValue::TFloat:
            s += Serializer<float>::size(v.value().getAsFloat());
            break;
        case script::PrimitiveValue::TString:
            s += Serializer<std::string>::size(v.value().getAsString());
            break;
        case script::PrimitiveValue::TArray:
            s += sizeof(std::uint32_t);
            for (const script::Value& av : v.value().getAsArray()) {
                s += Serializer<script::Value>::size(av);
            }
            break;
        default:
            break;
        }

        s += sizeof(std::uint32_t);
        for (const auto& prop : v.allProperties()) {
            s += Serializer<std::string>::size(prop.first);
            s += Serializer<script::Value>::size(prop.second.deref());
        }

        return s;
    }

private:
    static bool supported(script::PrimitiveValue::Type type) {
        switch (type) {
        case script::PrimitiveValue::TBool:
        case script::PrimitiveValue::TInteger:
        case script::PrimitiveValue::TFloat:
        case script::PrimitiveValue::TString:
        case script::PrimitiveValue::TArray:
            return true;
        default:
            return false;
        }
    }
};

template<>
struct Serializer<sf::Image, false> {
    static void serialize(stream::OutputStream& stream, const sf::Image& image) {
        const auto data = image.saveToMemory("png");
        if (!data.has_value()) { throw SerializationError("Failed to encode image as PNG"); }

        detail::OutputStreamWrapper wrapper(stream);
        wrapper.write<std::uint32_t>(data.value().size());
        stream.write(data.value().data(), data.value().size());
    }

    static void deserialize(stream::InputStream& stream, sf::Image& image) {
        detail::InputStreamWrapper wrapper(stream);
        std::uint32_t size;
        wrapper.read<std::uint32_t>(size);
        std::vector<char> data(size);
        if (stream.read(data.data(), size) != size) {
            throw SerializationError("Failed to read image bytes from stream");
        }
        if (!image.loadFromMemory(data.data(), size)) {
            throw SerializationError("Failed to decode image bytes");
        }
    }

    static std::uint32_t size(const sf::Image& image) {
        const auto data = image.saveToMemory("png");
        if (!data.has_value()) { return sizeof(std::uint32_t); }
        return sizeof(std::uint32_t) + data.value().size();
    }
};

template<typename T>
struct Serializer<std::optional<T>, false> {
    static void serialize(stream::OutputStream& output, const std::optional<T>& opt) {
        detail::OutputStreamWrapper wrapper(output);
        wrapper.write<std::uint8_t>(opt.has_value() ? 1 : 0);
        if (opt.has_value()) { Serializer<T>::serialize(output, *opt); }
    }

    static void deserialize(stream::InputStream& input, std::optional<T>& opt) {
        detail::InputStreamWrapper wrapper(input);
        std::uint8_t hasValue;
        wrapper.read<std::uint8_t>(hasValue);
        if (hasValue) {
            opt.emplace();
            Serializer<T>::deserialize(input, *opt);
        }
        else { opt.reset(); }
    }

    static std::uint32_t size(const std::optional<T>& opt) {
        std::uint32_t s = sizeof(std::uint8_t);
        if (opt.has_value()) { s += Serializer<T>::size(*opt); }
        return s;
    }
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
