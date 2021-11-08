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
    getSignal(Event::TextEntered).willAlwaysCall(std::bind(&TextEntry::onInput, this, _1));
    getSignal(Event::KeyPressed).willAlwaysCall(std::bind(&TextEntry::onKeypress, this, _1));
    getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&TextEntry::onClicked, this, _1));
    getSignal(Event::RenderSettingsChanged).willAlwaysCall(std::bind(&TextEntry::recalcText, this));
    getSignal(Event::Moved).willAlwaysCall([this](const Event&, Element*) {
        renderText.setPosition(getPosition());
    });

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
    Element::update(dt);

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
    renderText.setPosition(getPosition());
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

    renderText.setString(input);
    recalcNewlines();
    fireChanged();
}

void TextEntry::onKeypress(const Event& action) {
    if (action.type() != Event::KeyPressed) return;

    if (action.key().code == sf::Keyboard::Right) {
        if (cursorPos < input.size()) ++cursorPos;
    }
    else if (action.key().code == sf::Keyboard::Left) {
        if (cursorPos > 0) --cursorPos;
    }
    else if (action.key().code == sf::Keyboard::Up) {
        if (currentLine > 0) cursorUp();
    }
    else if (action.key().code == sf::Keyboard::Down) {
        if (currentLine < newlines.size() - 2) cursorDown();
    }
    else if (action.key().code == sf::Keyboard::Home) {
        cursorPos = newlines[currentLine] + 1;
    }
    else if (action.key().code == sf::Keyboard::End) {
        cursorPos = newlines[currentLine + 1];
    }
    else if (action.key().code == sf::Keyboard::Delete) {
        if (cursorPos < input.size()) input.erase(cursorPos, 1);
        fireChanged();
    }
    else if (action.key().code == sf::Keyboard::Return) {
        if (newlines.size() - 1 < lineCount) {
            if (cursorPos < input.size())
                input.insert(cursorPos, 1, '\n');
            else
                input.push_back('\n');
            ++cursorPos;
        }
        fireChanged();
    }
    else if (action.key().control && action.key().code == sf::Keyboard::V) {
        const std::string c = sf::Clipboard::getString().toAnsiString();
        if (cursorPos < input.size())
            input.insert(cursorPos, c);
        else
            input += c;
        cursorPos += c.size();
        fireChanged();
    }

    recalcNewlines();
}

void TextEntry::onClicked(const Event& action) {
    if (action.type() != Event::LeftClicked) return;

    recalcText();
    const auto selectIndex = [this](unsigned int i) {
        cursorPos = i;
        recalcNewlines();
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
}

void TextEntry::cursorDown() {
    const int cpos    = cursorPos - newlines[currentLine];
    const int lineLen = newlines[currentLine + 2] - newlines[currentLine + 1];
    const int npos    = std::min(cpos, lineLen);
    cursorPos         = newlines[currentLine + 1] + npos;
    currentLine += 1;
}

void TextEntry::fireChanged() { fireSignal(Event(Event::ValueChanged, input)); }

} // namespace gui
} // namespace bl
