#include <BLIB/Interfaces/Utilities/GhostWriter.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <cmath>

namespace bl
{
namespace interface
{
GhostWriter::GhostWriter()
: speed(engine::Configuration::getOrDefault<float>("blib.interface.ghost_speed", 20.f))
, showing(0)
, residual(0.f) {}

GhostWriter::GhostWriter(const std::string& content)
: speed(engine::Configuration::getOrDefault<float>("blib.interface.ghost_speed", 20.f))
, content(content)
, showing(0)
, residual(0.f) {}

void GhostWriter::setContent(const std::string& c) {
    content  = c;
    showing  = 0;
    residual = 0.f;
}

const std::string& GhostWriter::getContent() const { return content; }

std::string_view GhostWriter::getVisible() const { return {content.c_str(), showing}; }

bool GhostWriter::update(float dt) {
    if (finished()) return false;

    residual += dt;
    const unsigned int a = std::floor(residual * speed);
    residual -= static_cast<float>(a) / speed;

    for (unsigned int i = 0; i < a; ++i) {
        do { ++showing; } while (showing < content.size() && std::isspace(content[showing]));
        if (finished()) break;
    }

    return a > 0;
}

bool GhostWriter::finished() const { return showing >= content.size(); }

void GhostWriter::showAll() { showing = content.size(); }

} // namespace interface
} // namespace bl
