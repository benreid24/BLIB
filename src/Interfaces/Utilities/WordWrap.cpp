#include <BLIB/Interfaces/Utilities/WordWrap.hpp>

namespace bl
{
namespace interface
{
void wordWrap(sf::Text& text, float maxWidth) {
    const std::string content = text.getString().toAnsiString();
    std::vector<std::string> words(1);
    words.reserve(content.size() / 4);
    for (const char c : content) {
        if (std::isspace(c)) {
            if (!words.back().empty()) words.emplace_back();
        }
        else {
            words.back().push_back(c);
        }
    }

    text.setString("");
    if (words.empty()) return;

    std::vector<std::string> lines(1, words.front());
    for (unsigned int i = 1; i < words.size(); ++i) {
        const std::string& word = words[i];
        text.setString(lines.back() + " " + word);
        if (text.getGlobalBounds().width > maxWidth) { lines.emplace_back(word); }
        else {
            lines.back() += " " + word;
        }
    }

    std::string wrapped;
    wrapped.reserve(content.size() + lines.size());
    wrapped = lines.front();
    for (unsigned int i = 1; i < lines.size(); ++i) { wrapped += "\n" + lines[i]; }
    text.setString(wrapped);
}

} // namespace interface
} // namespace bl
