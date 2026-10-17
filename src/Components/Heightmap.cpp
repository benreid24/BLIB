#include <BLIB/Components/Heightmap.hpp>

#include <cmath>

namespace bl
{
namespace com
{
Heightmap::Heightmap()
: offset(0.f, 0.f)
, size(0.f, 0.f) {}

void Heightmap::create(const glm::vec2& offset, const glm::vec2& size, float step) {
    this->offset = offset;
    this->size   = size;
    heightmap.setSize(static_cast<unsigned int>(std::ceil(size.x / step)),
                      static_cast<unsigned int>(std::ceil(size.y / step)));
}

void Heightmap::create(const ctr::Vector2D<float>& heightmap, const glm::vec2& offset,
                       const glm::vec2& size) {
    this->offset    = offset;
    this->size      = size;
    this->heightmap = heightmap;
}

void Heightmap::create(ctr::Vector2D<float>&& heightmap, const glm::vec2& offset,
                       const glm::vec2& size) {
    this->offset    = offset;
    this->size      = size;
    this->heightmap = std::move(heightmap);
}

float Heightmap::sampleHeight(const glm::vec2& position) const {
    const glm::vec2 normalPos = (position - offset) / size;
    if (normalPos.x < 0.f || normalPos.x > 1.f || normalPos.y < 0.f || normalPos.y > 1.f) {
        return 0.f;
    }

    const float xi        = normalPos.x * static_cast<float>(heightmap.getWidth() - 1);
    const float yi        = normalPos.y * static_cast<float>(heightmap.getHeight() - 1);
    const unsigned int x0 = static_cast<unsigned int>(std::floor(xi));
    const unsigned int y0 = static_cast<unsigned int>(std::floor(yi));
    const unsigned int x1 = std::min(x0 + 1, heightmap.getWidth() - 1);
    const unsigned int y1 = std::min(y0 + 1, heightmap.getHeight() - 1);
    const float xn        = xi - static_cast<float>(x0);
    const float yn        = yi - static_cast<float>(y0);
    const float h00       = heightmap(x0, y0);
    const float h10       = heightmap(x1, y0);
    const float h01       = heightmap(x0, y1);
    const float h11       = heightmap(x1, y1);
    return (h00 * (1.f - xn) + h10 * xn) * (1.f - yn) + (h01 * (1.f - xn) + h11 * xn) * yn;
}

} // namespace com
} // namespace bl
