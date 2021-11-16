#include <BLIB/Interfaces/GUI/Elements/TextEntry.hpp>

#include <BLIB/Interfaces/GUI/Renderers/RendererUtil.hpp>
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
, cursorTime(0)
, currentLine(0) {
    using namespace std::placeholders;
    getSignal(Event::TextEntered).willAlwaysCall(std::bind(&TextEntry::onInput, this, _1));
    getSignal(Event::KeyPressed).willAlwaysCall(std::bind(&TextEntry::onKeypress, this, _1));
    getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&TextEntry::onClicked, this, _1));
    getSignal(Event::RenderSettingsChanged).willAlwaysCall(std::bind(&TextEntry::refresh, this));
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&TextEntry::refresh, this));
    getSignal(Event::Moved).willAlwaysCall([this](const Event&, Element*) {
        renderText.setPosition(getPosition());
    });

    newlines.reserve(lineCount + 2);
    recalcText();
}

void TextEntry::setMode(Mode m) {
    mode = m;
    filter();
}

void TextEntry::setMaxInputLength(unsigned int ml) { maxInputLen = ml; }

const std::string& TextEntry::getInput() const { return input; }

void TextEntry::setInput(const std::string& s) {
    input = s;
    filter();
    if (cursorPos > input.size()) cursorPos = input.size();
    recalcNewlines();
    recalcOffset();
}

bool TextEntry::cursorVisible() const { return cursorShowing && visible() && hasFocus(); }

unsigned int TextEntry::getCursorPosition() const { return cursorPos; }

void TextEntry::update(float dt) {
    Element::update(dt);

    if (hasFocus()) {
        cursorTime += dt;
        while (cursorTime > CursorFlashPeriod) {
            cursorTime -= CursorFlashPeriod;
            cursorShowing = !cursorShowing;
        }
    }
    else {
        cursorTime    = 0.f;
        cursorShowing = false;
    }
}

sf::Vector2f TextEntry::minimumRequisition() const {
    resource::Resource<sf::Font>::Ref font = renderSettings().font.value_or(Font::get());
    const int csize     = renderSettings().characterSize.value_or(TextEntry::DefaultCharacterSize);
    const float spacing = font ? std::ceil(font->getLineSpacing(csize)) : csize;
    const float paddedSpacing = spacing * 1.2f;
    return {10.f, csize + paddedSpacing * static_cast<float>(lineCount)};
}

void TextEntry::doRender(sf::RenderTarget& target, sf::RenderStates states,
                         const Renderer& renderer) const {
    renderer.renderTextEntry(target, states, *this);
}

void TextEntry::recalcText() {
    renderText = RendererUtil::buildRenderText(input, getAcquisition(), renderSettings());
    renderText.setPosition(getPosition() - textOffset);
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
    renderText.setString(input);
    recalcNewlines();
    recalcOffset();
    fireChanged();
}

void TextEntry::onKeypress(const Event& action) {
    if (action.type() != Event::KeyPressed) return;

    if (action.key().code == sf::Keyboard::Right) {
        if (cursorPos < input.size()) ++cursorPos;
        cursorShowing = true;
        cursorTime    = 0.f;
    }
    else if (action.key().code == sf::Keyboard::Left) {
        if (cursorPos > 0) --cursorPos;
        cursorShowing = true;
        cursorTime    = 0.f;
    }
    else if (action.key().code == sf::Keyboard::Up) {
        if (currentLine > 0) cursorUp();
        cursorShowing = true;
        cursorTime    = 0.f;
    }
    else if (action.key().code == sf::Keyboard::Down) {
        if (currentLine < newlines.size() - 2) cursorDown();
        cursorShowing = true;
        cursorTime    = 0.f;
    }
    else if (action.key().code == sf::Keyboard::Home) {
        cursorPos     = newlines[currentLine] + 1;
        cursorShowing = true;
        cursorTime    = 0.f;
    }
    else if (action.key().code == sf::Keyboard::End) {
        cursorPos     = newlines[currentLine + 1];
        cursorShowing = true;
        cursorTime    = 0.f;
    }
    else if (action.key().code == sf::Keyboard::Delete) {
        if (cursorPos < input.size()) input.erase(cursorPos, 1);
        filter();
        fireChanged();
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
    }

    recalcNewlines();
    recalcOffset();
}

void TextEntry::onClicked(const Event& action) {
    if (action.type() != Event::LeftClicked) return;

    recalcText();
    const auto selectIndex = [this](unsigned int i) {
        cursorPos = i;
        recalcNewlines();
        recalcOffset();
    };

    // Determine which line we're on
    const float y = action.mousePosition().y - renderText.getGlobalBounds().top;
    if (y < 0.f) {
        selectIndex(0);
        return;
    }
    if (y > renderText.getGlobalBounds().top + renderText.getGlobalBounds().height) {
        selectIndex(input.size());
        return;
    }

    const float lineHeight = renderText.getFont()->getLineSpacing(renderText.getCharacterSize()) *
                             renderText.getLineSpacing();
    const unsigned int line =
        std::min(static_cast<std::size_t>(std::floor(y / lineHeight)) + 1, newlines.size() - 1);

    const float x = action.mousePosition().x - renderText.getGlobalBounds().left;
    if (x < 0.f) {
        selectIndex(newlines[line - 1] + 1);
        return;
    }
    for (int i = newlines[line - 1] + 1; i < newlines[line]; ++i) {
        const sf::Glyph& g = renderText.getFont()->getGlyph(
            renderText.getString()[i],
            renderText.getCharacterSize(),
            (renderText.getStyle() & sf::Text::Style::Bold) == sf::Text::Style::Bold,
            renderText.getOutlineThickness());
        const sf::Vector2f cpos = renderText.findCharacterPos(i);
        const sf::FloatRect bounds(cpos.x, cpos.y, g.bounds.left + g.bounds.width, lineHeight);
        if (bounds.contains(action.mousePosition())) {
            selectIndex(i);
            return;
        }
    }

    selectIndex(newlines[line]);
}

void TextEntry::cursorUp() {
    const int cpos    = cursorPos - newlines[currentLine];
    const int lineLen = newlines[currentLine] - newlines[currentLine - 1];
    const int npos    = std::min(cpos, lineLen);
    cursorPos         = newlines[currentLine - 1] + npos;
    currentLine -= 1;
    recalcOffset();
}

void TextEntry::cursorDown() {
    const int cpos    = cursorPos - newlines[currentLine];
    const int lineLen = newlines[currentLine + 2] - newlines[currentLine + 1];
    const int npos    = std::min(cpos, lineLen);
    cursorPos         = newlines[currentLine + 1] + npos;
    currentLine += 1;
    recalcOffset();
}

void TextEntry::fireChanged() { fireSignal(Event(Event::ValueChanged, input)); }

const sf::Vector2f& TextEntry::getTextOffset() const { return textOffset; }

void TextEntry::recalcOffset() {
    recalcText();
    if (renderText.getGlobalBounds().width <= getAcquisition().width &&
        renderText.getGlobalBounds().height <= getAcquisition().height) {
        textOffset = {0.f, 0.f};
        recalcText();
        return;
    }

    const sf::Vector2f cpos = renderText.findCharacterPos(cursorPos);
    const sf::Glyph& g      = renderText.getFont()->getGlyph(
        renderText.getString()[cursorPos],
        renderText.getCharacterSize(),
        (renderText.getStyle() & sf::Text::Style::Bold) == sf::Text::Style::Bold,
        renderText.getOutlineThickness());

    const float right = getAcquisition().left + getAcquisition().width;
    if (cpos.x < getAcquisition().left) { textOffset.x = cpos.x - renderText.getPosition().x; }
    else if (cpos.x > right) {
        textOffset.x += cpos.x - right + g.bounds.width + g.advance + 1.5f;
    }

    const float lineHeight = renderText.getFont()->getLineSpacing(renderText.getCharacterSize()) *
                             renderText.getLineSpacing();
    const float y      = static_cast<float>(currentLine) * lineHeight + renderText.getPosition().y;
    const float by     = y + lineHeight;
    const float bottom = getAcquisition().top + getAcquisition().height;
    if (y < getAcquisition().top) { textOffset.y = y - renderText.getPosition().y; }
    else if (by > bottom) {
        textOffset.y = static_cast<float>(currentLine - lineCount + 1) * lineHeight;
    }
}

void TextEntry::refresh() {
    recalcText();
    recalcOffset();
}

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

} // namespace gui
} // namespace bl
