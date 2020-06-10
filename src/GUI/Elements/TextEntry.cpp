#include <BLIB/GUI/Elements/TextEntry.hpp>

#include <BLIB/GUI/Renderers/RendererUtil.hpp>
#include <GUI/Data/Font.hpp>
#include <cmath>

#include <iostream>

namespace bl
{
namespace gui
{
namespace
{
int getCharacterHeight(const RenderSettings& settings) {
    Resource<sf::Font>::Ref font = settings.font.value_or(Font::get());
    if (font)
        return std::ceil(font->getLineSpacing(
            settings.characterSize.value_or(TextEntry::DefaultCharacterSize)));
    return settings.characterSize.value_or(TextEntry::DefaultCharacterSize);
}

std::list<unsigned int>::const_iterator selectLine(const std::list<unsigned int>& newlines,
                                                   unsigned int line) {
    unsigned int i = 0;
    for (auto iter = newlines.begin(); iter != newlines.end(); ++iter) {
        if (i == line) return iter;
    }
    return newlines.end();
}

unsigned int getNewlinePos(const std::list<unsigned int>& newlines, unsigned int cursorPos,
                           unsigned int curLine, unsigned int newLine, unsigned int totalLen) {
    // Find lines
    const auto curLineIter = selectLine(newlines, curLine);
    const auto newLineIter = selectLine(newlines, newLine);
    if (curLineIter == newlines.end() || newLineIter == newlines.end()) return cursorPos;

    // Get dest line length
    auto nextLineIter = newLineIter;
    ++nextLineIter;
    unsigned int lineLen = 0;
    if (nextLineIter != newlines.end())
        lineLen = *nextLineIter - *newLineIter;
    else
        lineLen = totalLen - *newLineIter;

    // Compute new pos
    int offset = cursorPos - *curLineIter;
    if (offset > lineLen) offset = lineLen;
    return *newLineIter + offset;
}

} // namespace

TextEntry::Ptr TextEntry::create(unsigned int lc, const std::string& g, const std::string& i) {
    return Ptr(new TextEntry(lc, g, i));
}

TextEntry::TextEntry(unsigned int lc, const std::string& g, const std::string& i)
: Element(g, i)
, lineCount(lc)
, cursorPos(0)
, cursorShowing(false)
, cursorTime(0)
, currentLine(0) {
    using namespace std::placeholders;
    getSignal(Action::TextEntered).willAlwaysCall(std::bind(&TextEntry::onInput, this, _1));
    getSignal(Action::KeyPressed).willAlwaysCall(std::bind(&TextEntry::onKeypress, this, _1));
    getSignal(Action::LeftClicked).willAlwaysCall(std::bind(&TextEntry::onClicked, this, _1));
    getSignal(Action::RenderSettingsChanged)
        .willAlwaysCall(std::bind(&TextEntry::recalcText, this));
    recalcText();
}

void TextEntry::setMaxInputLength(unsigned int ml) { maxInputLen = ml; }

const sf::String& TextEntry::getInput() const { return input; }

void TextEntry::setInput(const sf::String& s) {
    input = s;
    if (cursorPos > input.getSize()) cursorPos = input.getSize();
    recalcNewlines();
}

bool TextEntry::cursorVisible() const { return cursorShowing && visible() && hasFocus(); }

unsigned int TextEntry::getCursorPosition() const { return cursorPos; }

void TextEntry::update(float dt) {
    if (hasFocus()) {
        cursorTime += dt;
        while (cursorTime > CursorFlashPeriod) {
            cursorTime -= CursorFlashPeriod;
            cursorShowing = !cursorShowing;
        }
    }
    else {
        cursorTime    = 0;
        cursorShowing = false;
    }
}

sf::Vector2i TextEntry::minimumRequisition() const {
    return {10, getCharacterHeight(renderSettings()) * static_cast<int>(lineCount)};
}

void TextEntry::doRender(sf::RenderTarget& target, sf::RenderStates states,
                         Renderer::Ptr renderer) const {
    renderer->renderTextEntry(target, states, *this);
}

void TextEntry::recalcText() {
    renderText = RendererUtil::buildRenderText(
        input.toAnsiString(), getAcquisition(), renderSettings());
    renderText.setPosition(0, 0);
}

void TextEntry::recalcNewlines() {
    newlines.clear();
    currentLine = 0;

    unsigned int curLineLen = 0;
    const std::string s     = input.toAnsiString();
    for (unsigned int i = 0; i < s.size(); ++i) {
        if (s[i] == '\n') newlines.push_back(i);
        if (i == cursorPos) currentLine = newlines.size();
    }
}

void TextEntry::onInput(const Action& action) {
    if (action.type != Action::TextEntered) return;

    const uint32_t c = action.data.input;
    if (c == 8) { // backspace
        if (cursorPos > 0) {
            input.erase(cursorPos - 1);
            --cursorPos;
        }
    }
    else if (c >= ' ') {
        if (!maxInputLen.has_value() || input.getSize() < maxInputLen.value()) {
            input.insert(cursorPos, c);
            ++cursorPos;
        }
    }

    std::cout << "typed: " << c << " input: " << input.toAnsiString() << std::endl;

    renderText.setString(input);
    recalcNewlines();
}

void TextEntry::onKeypress(const Action& action) {
    if (action.type != Action::KeyPressed) return;

    if (action.data.key.code == sf::Keyboard::Right) {
        if (cursorPos < input.getSize()) {
            if (input[cursorPos] == '\n') ++currentLine;
            ++cursorPos;
        }
    }
    else if (action.data.key.code == sf::Keyboard::Left) {
        if (cursorPos > 0) {
            if (input[cursorPos - 1] == '\n') --currentLine;
            --cursorPos;
        }
    }
    else if (action.data.key.code == sf::Keyboard::Up) { // TODO - up/down not working
        if (currentLine > 0) {
            cursorPos = getNewlinePos(
                newlines, cursorPos, currentLine, currentLine - 1, input.getSize());
            --currentLine;
        }
    }
    else if (action.data.key.code == sf::Keyboard::Down) {
        if (currentLine < newlines.size()) {
            cursorPos = getNewlinePos(
                newlines, cursorPos, currentLine, currentLine + 1, input.getSize());
            ++currentLine;
        }
    }
    else if (action.data.key.code == sf::Keyboard::Delete) {
        if (cursorPos < input.getSize()) input.erase(cursorPos);
    }
    else if (action.data.key.code == sf::Keyboard::Return) {
        if (newlines.size() + 1 < lineCount) {
            if (cursorPos < input.getSize())
                input.insert(cursorPos, '\n');
            else
                input = input + '\n'; // TODO - doesn't get added to end
        }
    }
}

void TextEntry::onClicked(const Action& action) {
    if (action.type != Action::LeftClicked) return;

    const sf::Vector2f pos(getAcquisition().left, getAcquisition().top);
    const sf::Vector2f mpos = action.position - pos;

    float minDist   = 10000000;
    unsigned int mi = 0;
    for (unsigned int i = 0; i < input.getSize(); ++i) {
        const sf::Vector2f cpos = renderText.findCharacterPos(i);
        const sf::Vector2f diff = cpos - mpos;
        const float d           = diff.x * diff.x + diff.y * diff.y;
        if (d < minDist) {
            minDist = d;
            mi      = i;
        }
    }

    cursorPos = mi;
    recalcNewlines();
}

} // namespace gui
} // namespace bl