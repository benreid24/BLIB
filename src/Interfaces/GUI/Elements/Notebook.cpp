#include <BLIB/Interfaces/GUI/Elements/Notebook.hpp>

#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>
#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr float TabTopPadding  = 2.f;
constexpr float ContentPadding = 2.f;
} // namespace

Notebook::Ptr Notebook::create() {
    Ptr nb(new Notebook());
    nb->add(nb->tabArea);
    return nb;
}

Notebook::Notebook()
: Container()
, tabArea(Box::create(LinePacker::create()))
, activePage(nullptr)
, maxWidth(-1.f)
, scroll(0.f) {
    tabArea->setExpandsWidth(true);
    tabArea->setExpandsHeight(true);
    tabArea->setOutlineThickness(0.f);
    getSignal(Event::Moved).willAlwaysCall(std::bind(&Notebook::onMove, this));
}

void Notebook::setMaxTabWidth(float mw) {
    maxWidth = mw;
    makeDirty();
}

Notebook::Page::Page(const std::string& name, const Label::Ptr& label, const Element::Ptr& content,
                     const Notebook::PageChangedCb& op, const Notebook::PageChangedCb& oc)
: name(name)
, label(label)
, content(content)
, onOpen(op)
, onClose(oc) {}

void Notebook::addPage(const std::string& name, const std::string& title,
                       const Element::Ptr& content, const PageChangedCb& onOpen,
                       const PageChangedCb& onClose) {
    if (pageMap.find(name) == pageMap.end()) {
        Label::Ptr label = Label::create(title);
        label->setRequisition(label->getRequisition() + sf::Vector2f(6.f, 6.f));
        const auto it = pages.emplace(pages.end(), name, label, content, onOpen, onClose);
        Page& p       = *it;
        p.content->setVisible(false);
        p.content->setExpandsWidth(true);
        p.content->setExpandsHeight(true);
        add(p.content);
        tabArea->pack(p.label, false, true);
        p.label->getSignal(Event::LeftClicked)
            .willAlwaysCall(std::bind(&Notebook::makePageActiveDirect, this, &p));

        if (pages.size() == 1) makePageActiveDirect(&*pages.begin());
    }
}

Notebook::Page* Notebook::getActivePage() const { return activePage; }

unsigned int Notebook::getActivePageIndex() const {
    unsigned int i = 0;
    for (auto it = pages.begin(); it != pages.end(); ++it, ++i) {
        if (&*it == activePage) return i;
    }
    return 0;
}

unsigned int Notebook::pageCount() const { return pages.size(); }

const std::list<Notebook::Page>& Notebook::getPages() const { return pages; }

const std::string& Notebook::getActivePageName() const {
    static const std::string empty;
    return activePage ? activePage->name : empty;
}

Notebook::Page* Notebook::getPageByIndex(unsigned int i) {
    auto it = getIterator(i);
    return it != pages.end() ? &(*it) : nullptr;
}

Notebook::Page* Notebook::getPageByName(const std::string& name) {
    const auto it = pageMap.find(name);
    return it != pageMap.end() ? &(*it->second) : nullptr;
}

void Notebook::removePageByIndex(unsigned int i) {
    if (i < pages.size()) {
        const auto it = getIterator(i);
        it->label->remove();
        it->content->remove();
        pageMap.erase(it->name);
        if (pages.size() > 1) {
            auto ni = it;
            if (i > 0) { --ni; }
            else { ++ni; }
            makePageActiveDirect(&*ni);
        }
        else { activePage = nullptr; }
        pages.erase(it);
    }
}

void Notebook::removePageByName(const std::string& name) {
    const auto it = pageMap.find(name);
    if (it != pageMap.end()) {
        auto pit = it->second;
        pit->label->remove();
        pit->content->remove();
        pageMap.erase(pit->name);
        if (pages.size() > 1) {
            auto ni = pit;
            if (ni != pages.begin()) { --ni; }
            else { ++ni; }
            makePageActiveDirect(&*ni);
        }
        else { activePage = nullptr; }
        pages.erase(pit);
    }
}

const sf::FloatRect& Notebook::getTabAcquisition() const { return tabAcquisition; }

sf::Vector2f Notebook::minimumRequisition() const {
    sf::Vector2f tabReq = tabArea->getRequisition();
    tabReq.x            = maxWidth > 0.f ? std::min(maxWidth, tabReq.x) : tabReq.x;
    tabReq.y += TabTopPadding;
    sf::Vector2f contentReq(0, 0);
    for (const Page& p : pages) {
        const sf::Vector2f creq = p.content->getRequisition();
        contentReq.x            = std::max(contentReq.x, creq.x);
        contentReq.y            = std::max(contentReq.y, creq.y);
    }
    contentReq += 2.f * sf::Vector2f(ContentPadding, ContentPadding);
    return {std::max(tabReq.x, contentReq.x), tabReq.y + contentReq.y};
}

void Notebook::onAcquisition() {
    const float contentOutline =
        activePage ?
            activePage->content->renderSettings().outlineThickness.value_or(TabTopPadding) :
            TabTopPadding;
    tabAcquisition = {getAcquisition().left + contentOutline,
                      getAcquisition().top + TabTopPadding,
                      std::max(tabArea->getRequisition().x, getAcquisition().width) -
                          contentOutline * 2.f,
                      tabArea->getRequisition().y - TabTopPadding};
    Packer::manuallyPackElement(tabArea, tabAcquisition, true);
    if (maxWidth > 0.f) {
        tabAcquisition.width =
            std::min(getAcquisition().width, tabAcquisition.width) - contentOutline * 2.f;
    }
    if (activePage) Packer::manuallyPackElement(activePage->content, contentArea());

    constrainScroll();
}

rdr::Component* Notebook::doPrepareRender(rdr::Renderer& renderer) {
    Container::doPrepareRender(renderer);
    return renderer.createComponent<Notebook>(*this, getWindowOrGuiParentComponent());
}

void Notebook::makePageActiveDirect(Page* page) {
    if (activePage) {
        activePage->content->setVisible(false, false);
        if (page != activePage && activePage) activePage->onClose();
    }
    page->content->setVisible(true, false);
    page->content->moveToTop();
    Packer::manuallyPackElement(page->content, contentArea(), true);
    if (page != activePage) page->onOpen();
    activePage = page;

    if (maxWidth > 0.f) {
        const float x = page->label->getPosition().x - scroll;
        if (x < getPosition().x) { scroll = page->label->getPosition().x - getPosition().x; }
        else {
            const float right = getAcquisition().left + getAcquisition().width;
            const float xr    = x + page->label->getAcquisition().width;
            if (xr > right) {
                scroll = (page->label->getPosition().x - getPosition().x) - getAcquisition().width +
                         page->label->getAcquisition().width;
            }
        }
        constrainScroll();
    }
}

void Notebook::makePageActive(unsigned int i) {
    if (i < pages.size()) {
        Page* np = &(*getIterator(i));
        makePageActiveDirect(np);
    }
}

void Notebook::onMove() {
    tabArea->recalculatePosition();
    for (Page& p : pages) { p.content->recalculatePosition(); }
    tabAcquisition.left = getPosition().x;
    tabAcquisition.top  = getPosition().y;
}

std::list<Notebook::Page>::iterator Notebook::getIterator(unsigned int i) {
    auto it = pages.begin();
    for (unsigned int j = 0; j < i; ++j) {
        if (it != pages.end()) { ++it; }
        else { break; }
    }
    return it;
}

sf::FloatRect Notebook::contentArea() const {
    return {getAcquisition().left + ContentPadding,
            getAcquisition().top + tabArea->getRequisition().y + ContentPadding,
            getAcquisition().width - 2.f * ContentPadding,
            getAcquisition().height - tabArea->getRequisition().y - 2.f * ContentPadding};
}

void Notebook::requestMakeDirty(const Element* child) {
    if (child->packable(true)) makeDirty();
}

bool Notebook::propagateEvent(const Event& e) {
    if (e.type() == Event::Scrolled && tabAcquisition.contains(e.mousePosition())) {
        scroll += e.scrollDelta() * -4.f;
        constrainScroll();
        const Event fakeMove(Event::MouseMoved,
                             sf::Vector2f(e.mousePosition().x + scroll, e.mousePosition().y));
        tabArea->processEvent(fakeMove);
        return true;
    }

    const Event translated(e, sf::Vector2f(e.mousePosition().x + scroll, e.mousePosition().y));
    if (tabArea->processEvent(translated)) return true;
    if (activePage) return activePage->content->processEvent(e);
    return false;
}

void Notebook::constrainScroll() {
    if (scroll < 0.f) { scroll = 0.f; }
    if (maxWidth > 0.f && tabArea->getAcquisition().width > getAcquisition().width) {
        const float ms = tabArea->getAcquisition().width - tabAcquisition.width;
        if (scroll > ms) { scroll = ms; }
    }

    if (maxWidth < 0.f) scroll = 0.f;
}

} // namespace gui
} // namespace bl
