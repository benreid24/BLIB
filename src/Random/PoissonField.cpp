#include <BLIB/Random/PoissonField.hpp>

#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Random/Random.hpp>

namespace bl
{
namespace rand
{
namespace
{
constexpr unsigned int SampleCount = 30;
constexpr float RadiansPerSample = 2.f * 3.14159265358979323846f / static_cast<float>(SampleCount);
} // namespace

PoissonField::PoissonField(const glm::vec2& size, float minDistance) {
    generate(size, minDistance);
}

// TODO - support custom seed
void PoissonField::generate(const glm::vec2& size, float r) {
    const float cellSize                = r / std::sqrt(2.f);
    const unsigned int estimatedSamples = std::ceil(size.x / r) * std::ceil(size.y / r);

    samplePoints.clear();
    samplePoints.reserve(estimatedSamples);

    ctr::Vector2D<std::int32_t> spatialMap(static_cast<unsigned int>(std::ceil(size.x / cellSize)),
                                           static_cast<unsigned int>(std::ceil(size.y / cellSize)),
                                           -1);
    std::vector<std::uint32_t> activeList;
    activeList.reserve(estimatedSamples);

    const auto sample = [&size, r](unsigned int i, const glm::vec2& center) {
        const float angle  = RadiansPerSample * static_cast<float>(i);
        const float radius = rand::Random::get<float>(r, 2.f * r);
        return glm::vec2(center.x + radius * std::cos(angle), center.y + radius * std::sin(angle));
    };

    const auto addPoint = [this, &spatialMap, &activeList, cellSize, r](const glm::vec2& point) {
        const unsigned int ix = static_cast<int>(point.x / cellSize);
        const unsigned int iy = static_cast<int>(point.y / cellSize);
        const unsigned int i  = samplePoints.size();
        spatialMap(ix, iy)    = i;
        samplePoints.emplace_back(point);
        activeList.emplace_back(i);
    };

    const auto checkNewPoint = [this, &spatialMap, &size, cellSize, r](const glm::vec2& point) {
        if (point.x < 0.f || point.x >= size.x || point.y < 0.f || point.y >= size.y) {
            return false;
        }

        const unsigned int ix   = static_cast<int>(point.x / cellSize);
        const unsigned int iy   = static_cast<int>(point.y / cellSize);
        const unsigned int minX = (ix > 1) ? ix - 2 : 0;
        const unsigned int maxX = std::min(ix + 2, spatialMap.getWidth() - 1);
        const unsigned int minY = (iy > 1) ? iy - 2 : 0;
        const unsigned int maxY = std::min(iy + 2, spatialMap.getHeight() - 1);
        for (unsigned int x = minX; x <= maxX; ++x) {
            for (unsigned int y = minY; y <= maxY; ++y) {
                const std::int32_t index = spatialMap(x, y);
                if (index >= 0) {
                    const glm::vec2& other = samplePoints[index];
                    if (glm::distance(point, other) < r) { return false; }
                }
            }
        }
        return true;
    };

    const glm::vec2 firstPoint(rand::Random::get<float>(0.f, size.x),
                               rand::Random::get<float>(0.f, size.y));
    addPoint(firstPoint);

    while (!activeList.empty()) {
        const unsigned int activeIndex = rand::Random::get<unsigned int>(0, activeList.size() - 1);
        const glm::vec2& activePoint   = samplePoints[activeList[activeIndex]];

        bool added = false;
        for (unsigned int i = 0; i < SampleCount; ++i) {
            const glm::vec2 newSample = sample(i, activePoint);
            if (checkNewPoint(newSample)) {
                addPoint(newSample);
                added = true;
                break;
            }
        }
        if (!added) { activeList.erase(activeList.begin() + activeIndex); }
    }
}

} // namespace rand
} // namespace bl
