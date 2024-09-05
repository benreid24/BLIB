#ifndef BLIB_RENDER_COLOR_HPP
#define BLIB_RENDER_COLOR_HPP

#include <BLIB/Serialization/Binary/Serializer.hpp>
#include <BLIB/Serialization/JSON/Serializer.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <initializer_list>
#include <stdexcept>

namespace bl
{
namespace rc
{
/**
 * @brief Basic color class meant to wrap sf::Color and glm::vec4 to provide interoperability
 *
 * @ingroup Renderer
 */
class Color {
public:
    /**
     * @brief Makes an opaque white color
     */
    Color()
    : color(1.f) {}

    /**
     * @brief Creates the color from the given normalized color
     *
     * @param color The normalized color
     */
    Color(const glm::vec4& color)
    : color(color) {}

    /**
     * @brief Creates the color from an sf::Color object
     *
     * @param color The color to be
     */
    Color(const sf::Color& color) { *this = color; }

    /**
     * @brief Creates the color from rgba channels in range [0,255]
     *
     * @param r The red channel
     * @param g The green channel
     * @param b The blue channel
     * @param a The alpha channel
     */
    Color(int r, int g, int b, int a = 255) { *this = sf::Color(r, g, b, a); }

    /**
     * @brief Creates the color from normalized rgba channels [0,1]
     *
     * @param r The red channel
     * @param g The green channel
     * @param b The blue channel
     * @param a The alpha channel
     */
    Color(float r, float g, float b, float a = 1.f)
    : color(r, g, b, a) {}

    /**
     * @brief Creates the color from normalized rgba channels [0,1]
     *
     * @param rgba An initializer list containing 3 or 4 channels in rgba order
     */
    Color(std::initializer_list<float> rgba) {
        if (rgba.size() < 3 || rgba.size() > 4) {
            throw std::runtime_error("Invalid number of color channels");
        }

        color.x = *rgba.begin();
        color.y = *(rgba.begin() + 1);
        color.z = *(rgba.begin() + 2);
        color.w = rgba.size() == 4 ? *(rgba.begin() + 3) : 1.f;
    }

    /**
     * @brief Creates the color from rgba channels [0,255]
     *
     * @param rgba An initializer list containing 3 or 4 channels in rgba order
     */
    Color(std::initializer_list<int> rgba) {
        if (rgba.size() < 3 || rgba.size() > 4) {
            throw std::runtime_error("Invalid number of color channels");
        }

        *this = sf::Color(*rgba.begin(),
                          *(rgba.begin() + 1),
                          *(rgba.begin() + 2),
                          rgba.size() == 4 ? *(rgba.begin() + 3) : 255);
    }

    /**
     * @brief Converts the color to a glm::vec4
     */
    operator glm::vec4() const { return color; }

    /**
     * @brief Converts the color to an sf::Color
     */
    operator sf::Color() const { return toSfColor(); }

    /**
     * @brief Converts the color to a glm::vec4
     */
    glm::vec4 toVec4() const { return color; }

    /**
     * @brief Converts the color to an sf::Color
     */
    sf::Color toSfColor() const {
        return sf::Color(color.x * 255.f, color.y * 255.f, color.z * 255.f, color.w * 255.f);
    }

    /**
     * @brief Updates the color from a glm::vec4
     *
     * @param newColor The new color to be
     * @return A reference to this color
     */
    Color& operator=(const glm::vec4& newColor) {
        color = newColor;
        return *this;
    }

    /**
     * @brief Updates the color from an sf::Color
     *
     * @param newColor The new color to be
     * @return A reference to this color
     */
    Color& operator=(const sf::Color& newColor) {
        color.x = static_cast<float>(newColor.r) / 255.f;
        color.y = static_cast<float>(newColor.g) / 255.f;
        color.z = static_cast<float>(newColor.b) / 255.f;
        color.w = static_cast<float>(newColor.a) / 255.f;
        return *this;
    }

private:
    glm::vec4 color;
};

} // namespace rc

namespace serial
{
namespace binary
{
template<>
struct Serializer<rc::Color> {
    using ChannelSerial = Serializer<std::uint8_t>;

    static bool serialize(OutputStream& output, const rc::Color& v) {
        const sf::Color col = v.toSfColor();
        return ChannelSerial::serialize(output, col.r) && ChannelSerial::serialize(output, col.g) &&
               ChannelSerial::serialize(output, col.b) && ChannelSerial::serialize(output, col.a);
    }

    static bool deserialize(InputStream& input, rc::Color& v) {
        std::uint8_t r, g, b, a;
        if (!ChannelSerial::deserialize(input, r) && ChannelSerial::deserialize(input, g) &&
            ChannelSerial::deserialize(input, b) && ChannelSerial::deserialize(input, a)) {
            return false;
        }

        v = sf::Color(r, g, b, a);
        return true;
    }

    static std::uint32_t size(const rc::Color& v) { return ChannelSerial::size(0) * 4; }
};
} // namespace binary

namespace json
{
template<>
struct Serializer<rc::Color> {
    using VecSerial = Serializer<glm::vec4>;

    static Value serialize(const rc::Color& v) { return VecSerial::serialize(v.toVec4()); }

    static void serializeInto(const std::string& key, Group& g, const rc::Color& val) {
        priv::Serializer<rc::Color>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(rc::Color& result, const Value& val) {
        glm::vec4 parsed;
        if (!VecSerial::deserialize(parsed, val)) { return false; }
        result = parsed;
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, rc::Color& result) {
        return priv::Serializer<rc::Color>::deserializeFrom(val, key, result, &deserialize);
    }

    static bool deserializeStream(std::istream& stream, rc::Color& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const rc::Color& value, unsigned int tab,
                                unsigned int indent) {
        return VecSerial::serializeStream(stream, value.toVec4(), tab, indent);
    }
};
} // namespace json
} // namespace serial
} // namespace bl

#endif
