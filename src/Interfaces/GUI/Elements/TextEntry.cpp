#include <BLIB/Interfaces/GUI/Elements/TextEntry.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>
#include <Interfaces/GUI/Data/Font.hpp>
#include <cmath>
#include <type_traits>

namespace bl
{
namespace gui
{
namespace
{
inline bool modeEnabled(TextEntry::Mode mode, TextEntry::Mode check) {
    using T   = std::underlying_type_t<TextEntry::Mode>;
    const T m = static_cast<T>(mode);
    const T c = static_cast<T>(check);
    return (m & c) != 0;
}
} // namespace

TextEntry::Ptr TextEntry::create(unsigned int lc, bool am) { return Ptr(new TextEntry(lc, am)); }

TextEntry::TextEntry(unsigned int lc, bool am)
: Element()
, lineCount(lc)
, allowMoreLines(am)
, mode(Mode::Any)
, cursorPos(0)
, cursorShowing(false)
, currentLine(0) {
    using namespace std::placeholders;
    getSignal(Event::TextEntered).willAlwaysCall(std::bind(&TextEntry::onInput, this, _1));
    getSignal(Event::KeyPressed).willAlwaysCall(std::bind(&TextEntry::onKeypress, this, _1));
    getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&TextEntry::onClicked, this, _1));
    getSignal(Event::GainedFocus).willAlwaysCall(std::bind(&TextEntry::updateCursorState, this));
    getSignal(Event::LostFocus).willAlwaysCall(std::bind(&TextEntry::updateCursorState, this));

    newlines.reserve(lineCount + 2);
}

void TextEntry::refreshComponent() {
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

void TextEntry::setMode(Mode m) {
    mode = m;
    filter();
    refreshComponent();
}

void TextEntry::setMaxInputLength(unsigned int ml) { maxInputLen = ml; }

const std::string& TextEntry::getInput() const { return input; }

void TextEntry::setInput(const std::string& s) {
    input = s;
    filter();
    if (cursorPos > input.size()) cursorPos = input.size();
    recalcNewlines();
    refreshComponent();
}

bool TextEntry::cursorVisible() const { return cursorShowing && visible() && hasFocus(); }

unsigned int TextEntry::getCursorPosition() const { return cursorPos; }

void TextEntry::updateCursorState() {
    if (hasFocus() != cursorShowing) {
        cursorShowing = hasFocus();
        if (component) { component->onCaratStateUpdate(); }
    }
}

sf::Vector2f TextEntry::minimumRequisition() const {
    resource::Ref<sf::VulkanFont> font = renderSettings().font.value_or(Font::get());
    const int csize     = renderSettings().characterSize.value_or(TextEntry::DefaultCharacterSize);
    const float spacing = font ? std::ceil(font->getLineSpacing(csize)) : csize;
    const float paddedSpacing = spacing * 1.2f;
    return {10.f, csize + paddedSpacing * static_cast<float>(lineCount)};
}

rdr::Component* TextEntry::doPrepareRender(rdr::Renderer& renderer) {
    rdr::Component* com = renderer.createComponent<TextEntry>(*this);
    component           = dynamic_cast<rdr::TextEntryComponentBase*>(com);
    if (!component) {
        BL_LOG_ERROR << "TextEntry component must derive from TextEntryComponentBase";
    }
    return com;
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

void TextEntry::onInput(const Event& action) {
    if (action.type() != Event::TextEntered) return;

    const uint32_t c = action.character();
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

    filter();
    recalcNewlines();
    refreshComponent();
    fireChanged();
}

void TextEntry::showAndResetCursor() {
    cursorShowing = true;
    if (component) {
        component->onCaratStateUpdate();
        component->resetCaratFlash();
    }
}

void TextEntry::onKeypress(const Event& action) {
    if (action.type() != Event::KeyPressed) return;

    if (action.key().code == sf::Keyboard::Right) {
        if (cursorPos < input.size()) ++cursorPos;
        showAndResetCursor();
    }
    else if (action.key().code == sf::Keyboard::Left) {
        if (cursorPos > 0) --cursorPos;
        showAndResetCursor();
    }
    else if (action.key().code == sf::Keyboard::Up) {
        if (currentLine > 0) cursorUp();
        showAndResetCursor();
    }
    else if (action.key().code == sf::Keyboard::Down) {
        if (currentLine < newlines.size() - 2) cursorDown();
        showAndResetCursor();
    }
    else if (action.key().code == sf::Keyboard::Home) {
        cursorPos = newlines[currentLine] + 1;
        showAndResetCursor();
    }
    else if (action.key().code == sf::Keyboard::End) {
        cursorPos = newlines[currentLine + 1];
        showAndResetCursor();
    }
    else if (action.key().code == sf::Keyboard::Delete) {
        if (cursorPos < input.size()) input.erase(cursorPos, 1);
        filter();
        fireChanged();
        refreshComponent();
    }
    else if (action.key().code == sf::Keyboard::Return) {
        if (newlines.size() - 1 < lineCount || allowMoreLines) {
            if (cursorPos < input.size())
                input.insert(cursorPos, 1, '\n');
            else
                input.push_back('\n');
            ++cursorPos;
        }
        filter();
        fireChanged();
        refreshComponent();
    }
    else if (action.key().control && action.key().code == sf::Keyboard::V) {
        const std::string c = sf::Clipboard::getString().toAnsiString();
        if (cursorPos < input.size())
            input.insert(cursorPos, c);
        else
            input += c;
        cursorPos += c.size();
        filter();
        fireChanged();
        refreshComponent();
    }

    recalcNewlines();
}

void TextEntry::onClicked(const Event& action) {
    if (action.type() != Event::LeftClicked) return;
    if (!component) {
        BL_LOG_WARN << "Received click event without valid component";
        return;
    }

    cursorPos = component->findCursorPosition(action.mousePosition());
    recalcNewlines();
    component->onCaratStateUpdate();
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

void TextEntry::fireChanged() { fireSignal(Event(Event::ValueChanged, input)); }

void TextEntry::filter() {
    if (mode == Mode::Any) return;

    if (modeEnabled(mode, Mode::Integer) || modeEnabled(mode, Mode::Float)) {
        bool decimalFound = false;
        for (int i = 0; i < static_cast<int>(input.size()); ++i) {
            if (!std::isdigit(input[i])) {
                if (i == 0 && input[i] == '-' && modeEnabled(mode, Mode::Signed)) continue;
                if (input[i] == '.' && modeEnabled(mode, Mode::Float) && !decimalFound) {
                    decimalFound = true;
                    continue;
                }
                input.erase(i, 1);
                --i;
                --cursorPos;
            }
        }
        if (input.empty()) { input = "0"; }
        if (cursorPos > input.size()) cursorPos = input.size();
    }
}

unsigned int TextEntry::getCurrentLine() const { return currentLine; }

unsigned int TextEntry::getLineCount() const { return lineCount; }

unsigned int TextEntry::getLineStartIndex(unsigned int line) const {
    if (newlines.size() <= 1) { return 0; }

    line = line >= newlines.size() ? newlines.size() - 1 : line;
    return newlines[line - 1] + 1;
}

unsigned int TextEntry::getLineEndIndex(unsigned int line) const {
    if (newlines.size() <= 1) { return input.size(); }

    line = line >= newlines.size() ? newlines.size() - 1 : line;
    return newlines[line];
}

} // namespace gui
} // namespace bl
