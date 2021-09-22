#include <BLIB/Interfaces/GUI/Elements/TextEntry.hpp>

#include <BLIB/Interfaces/GUI/Renderers/RendererUtil.hpp>
#include <Interfaces/GUI/Data/Font.hpp>
#include <cmath>

namespace bl
{
namespace gui
{
TextEntry::Ptr TextEntry::create(unsigned int lc) { return Ptr(new TextEntry(lc)); }

TextEntry::TextEntry(unsigned int lc)
: Element()
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

    newlines.reserve(lineCount + 2);
    recalcText();
}

void TextEntry::setMaxInputLength(unsigned int ml) { maxInputLen = ml; }

const std::string& TextEntry::getInput() const { return input; }

void TextEntry::setInput(const std::string& s) {
    input = s;
    if (cursorPos > input.size()) cursorPos = input.size();
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
    resource::Resource<sf::Font>::Ref font = renderSettings().font.value_or(Font::get());
    const int csize     = renderSettings().characterSize.value_or(TextEntry::DefaultCharacterSize);
    const float spacing = font ? std::ceil(font->getLineSpacing(csize)) : csize;
    const int paddedSpacing = spacing * 1.2f;
    return {10, csize + paddedSpacing * static_cast<int>(lineCount)};
}

void TextEntry::doRender(sf::RenderTarget& target, sf::RenderStates states,
                         const Renderer& renderer) const {
    renderer.renderTextEntry(target, states, *this);
}

void TextEntry::recalcText() {
    renderText = RendererUtil::buildRenderText(input, getAcquisition(), renderSettings());
    renderText.setPosition(0, 0);
}

void TextEntry::recalcNewlines() {
    newlines.clear();
    currentLine = 0;
    if (cursorPos > input.size()) cursorPos = input.size();

    newlines.push_back(-1);
    for (unsigned int i = 0; i < input.size(); ++i) {
        if (i == cursorPos) currentLine = newlines.size() - 1;
        if (input[i] == '\n') newlines.push_back(i);
    }
    if (cursorPos == input.size()) currentLine = newlines.size() - 1;
    newlines.push_back(input.size());
}

void TextEntry::onInput(const Action& action) {
    if (action.type != Action::TextEntered) return;

    const uint32_t c = action.data.input;
    if (c == 8) { // backspace
        if (cursorPos > 0) {
            input.erase(cursorPos - 1, 1);
            --cursorPos;
        }
    }
    else if (c >= ' ') {
        if (!maxInputLen.has_value() || input.size() < maxInputLen.value()) {
            input.insert(cursorPos, 1, c);
            ++cursorPos;
        }
    }

    renderText.setString(input);
    recalcNewlines();
}

void TextEntry::onKeypress(const Action& action) {
    if (action.type != Action::KeyPressed) return;

    if (action.data.key.code == sf::Keyboard::Right) {
        if (cursorPos < input.size()) ++cursorPos;
    }
    else if (action.data.key.code == sf::Keyboard::Left) {
        if (cursorPos > 0) --cursorPos;
    }
    else if (action.data.key.code == sf::Keyboard::Up) {
        if (currentLine > 0) cursorUp();
    }
    else if (action.data.key.code == sf::Keyboard::Down) {
        if (currentLine < newlines.size() - 2) cursorDown();
    }
    else if (action.data.key.code == sf::Keyboard::Home) {
        cursorPos = newlines[currentLine] + 1;
    }
    else if (action.data.key.code == sf::Keyboard::End) {
        cursorPos = newlines[currentLine + 1];
    }
    else if (action.data.key.code == sf::Keyboard::Delete) {
        if (cursorPos < input.size()) input.erase(cursorPos, 1);
    }
    else if (action.data.key.code == sf::Keyboard::Return) {
        if (newlines.size() - 1 < lineCount) {
            if (cursorPos < input.size())
                input.insert(cursorPos, 1, '\n');
            else
                input.push_back('\n');
            ++cursorPos;
        }
    }
    else if (action.data.key.control && action.data.key.code == sf::Keyboard::V) {
        const std::string c = sf::Clipboard::getString().toAnsiString();
        if (cursorPos < input.size())
            input.insert(cursorPos, c);
        else
            input += c;
        cursorPos += c.size();
    }

    recalcNewlines();
}

void TextEntry::onClicked(const Action& action) {
    if (action.type != Action::LeftClicked) return;

    const sf::Vector2f pos(getAcquisition().left, getAcquisition().top);
    const sf::Vector2f mpos = action.position - pos;

    float minDist   = 10000000;
    unsigned int mi = 0;
    for (unsigned int i = 0; i < input.size(); ++i) {
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

void TextEntry::cursorUp() {
    const int cpos    = cursorPos - newlines[currentLine];
    const int lineLen = newlines[currentLine] - newlines[currentLine - 1];
    const int npos    = std::min(cpos, lineLen);
    cursorPos         = newlines[currentLine - 1] + npos;
    currentLine -= 1;
}

void TextEntry::cursorDown() {
    const int cpos    = cursorPos - newlines[currentLine];
    const int lineLen = newlines[currentLine + 2] - newlines[currentLine + 1];
    const int npos    = std::min(cpos, lineLen);
    cursorPos         = newlines[currentLine + 1] + npos;
    currentLine += 1;
}

} // namespace gui
} // namespace bl