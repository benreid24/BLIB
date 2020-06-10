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

int selectLine(const std::list<unsigned int>& newlines, unsigned int line) {
    if (line == 0) return 0;

    int i = 0;
    for (auto iter = newlines.begin(); iter != newlines.end(); ++iter) {
        if (i + 1 == line) {
            std::cout << "found line " << i << std::endl;
            return *iter;
        }
        ++i;
    }
    return -1;
}

unsigned int getNewlinePos(const std::list<unsigned int>& newlines, unsigned int cursorPos,
                           unsigned int curLine, unsigned int newLine, unsigned int totalLen) {
    std::cout << "going from line " << curLine << " to " << newLine << std::endl;

    // Find lines
    const int curLineStart = selectLine(newlines, curLine);
    const int newLineStart = selectLine(newlines, newLine);
    if (curLineStart < 0 || newLineStart < 0) return cursorPos;

    // Get dest line length
    const int nextLineStart = selectLine(newlines, newLine + 1);

    std::cout << "current line: " << curLineStart << std::endl;
    std::cout << "new line: " << newLineStart << std::endl;
    std::cout << "next line: " << nextLineStart << std::endl;

    unsigned int lineLen = 0;
    if (nextLineStart >= 0)
        lineLen = nextLineStart - newLineStart;
    else
        lineLen = totalLen - newLineStart;

    std::cout << "new line len: " << lineLen << std::endl;

    // Compute new pos
    int offset = cursorPos - curLineStart;
    std::cout << "offset: " << offset << std::endl;
    if (offset > lineLen) offset = lineLen;
    std::cout << "adjusted: " << offset << std::endl;
    std::cout << "cursor: " << cursorPos << " -> " << (newLineStart + offset) << std::endl;
    return newLineStart + offset;
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

    const std::string s = input.toAnsiString();
    for (unsigned int i = 0; i < s.size(); ++i) {
        if (i == cursorPos) currentLine = newlines.size();
        if (s[i] == '\n') newlines.push_back(i);
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
        if (cursorPos < input.getSize()) ++cursorPos;
    }
    else if (action.data.key.code == sf::Keyboard::Left) {
        if (cursorPos > 0) --cursorPos;
    }
    else if (action.data.key.code == sf::Keyboard::Up) { // TODO - up/down not working
        if (currentLine > 0) {
            cursorPos = getNewlinePos(
                newlines, cursorPos, currentLine, currentLine - 1, input.getSize());
        }
    }
    else if (action.data.key.code == sf::Keyboard::Down) {
        if (currentLine < newlines.size()) {
            cursorPos = getNewlinePos(
                newlines, cursorPos, currentLine, currentLine + 1, input.getSize());
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

    recalcNewlines();
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