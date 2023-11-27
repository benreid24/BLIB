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
, residual(0.f)
, managing(nullptr) {}

GhostWriter::GhostWriter(const std::string& content)
: speed(engine::Configuration::getOrDefault<float>("blib.interface.ghost_speed", 20.f))
, content(content)
, showing(0)
, residual(0.f)
, managing(nullptr) {}

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
    const unsigned int a = static_cast<int>(std::floor(residual * speed));
    residual -= static_cast<float>(a) / speed;

    for (unsigned int i = 0; i < a; ++i) {
        do { ++showing; } while (showing < content.size() && std::isspace(content[showing]));
        if (finished()) break;
    }

    if (a > 0) {
        syncText();
        return true;
    }
    return false;
}

bool GhostWriter::finished() const { return showing >= content.size(); }

void GhostWriter::showAll() {
    showing = static_cast<unsigned int>(content.size());
    syncText();
}

void GhostWriter::manage(gfx::Text& t) {
    managing = &t.getSection();
    syncText();
}

void GhostWriter::manage(gfx::txt::BasicText& s) {
    managing = &s;
    syncText();
}

void GhostWriter::stopManaging() { managing = nullptr; }

void GhostWriter::syncText() {
    if (managing) { managing->setString(content.substr(0, showing)); }
}

} // namespace interface
} // namespace bl
