#include <BLIB/Interfaces/GUI/Elements/Notebook.hpp>

#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>
#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>

namespace bl
{
namespace gui
{
Notebook::Ptr Notebook::create() {
    Ptr nb(new Notebook());
    nb->add(nb->tabArea);
    return nb;
}

Notebook::Notebook()
: Container()
, tabArea(Box::create(LinePacker::create()))
, activePage(nullptr)
, activePageIndex(0) {
    tabArea->setExpandsWidth(true);
    tabArea->setExpandsHeight(true);
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Notebook::onAcquisition, this));
    getSignal(Event::Moved).willAlwaysCall(std::bind(&Notebook::onMove, this));
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
        p.content->skipPacking(true);
        p.content->setVisible(false);
        p.content->setExpandsWidth(true);
        p.content->setExpandsHeight(true);
        add(p.content);
        tabArea->pack(p.label, false, true);
        p.label->getSignal(Event::LeftClicked)
            .willAlwaysCall(std::bind(&Notebook::pageClicked, this, &p));

        if (pages.size() == 1) makePageActive(0);
    }
}

Notebook::Page* Notebook::getActivePage() const { return activePage; }

unsigned int Notebook::getActivePageIndex() const { return activePageIndex; }

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
        if (i > 0) makePageActive(i - 1);
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
        if (pages.size() > 1) makePageActive(0);
        pages.erase(pit);
    }
}

const sf::FloatRect& Notebook::getTabAcquisition() const { return tabArea->getAcquisition(); }

sf::Vector2f Notebook::minimumRequisition() const {
    const sf::Vector2f tabReq = tabArea->getRequisition();
    sf::Vector2f contentReq(0, 0);
    for (const Page& p : pages) {
        const sf::Vector2f creq = p.content->getRequisition();
        contentReq.x            = std::max(contentReq.x, creq.x);
        contentReq.y            = std::max(contentReq.y, creq.y);
    }
    return {std::max(tabReq.x, contentReq.x), tabReq.y + contentReq.y};
}

void Notebook::onAcquisition() {
    Packer::manuallyPackElement(tabArea,
                                {getAcquisition().left,
                                 getAcquisition().top,
                                 getAcquisition().width,
                                 tabArea->getRequisition().y});
    if (activePage) Packer::manuallyPackElement(activePage->content, contentArea());
}

void Notebook::doRender(sf::RenderTarget& target, sf::RenderStates states,
                        const Renderer& renderer) const {
    renderer.renderNotebookTabs(target, states, *this);
    if (activePage) activePage->content->render(target, states, renderer);
}

void Notebook::pageClicked(Page* page) {
    unsigned int i = 0;
    for (const Page& p : pages) {
        if (&p == page) {
            makePageActive(i);
            break;
        }
        ++i;
    }
}

void Notebook::makePageActive(unsigned int i) {
    if (i < pages.size()) {
        Page* np = &(*getIterator(i));
        if (activePage) {
            activePage->content->setVisible(false, false);
            if (np != activePage && activePage) activePage->onClose();
        }
        np->content->setVisible(true, false);
        np->content->moveToTop();
        Packer::manuallyPackElement(np->content, contentArea());
        if (np != activePage) np->onOpen();
        activePage = np;
    }
}

void Notebook::onMove() {
    tabArea->recalculatePosition();
    for (Page& p : pages) { p.content->recalculatePosition(); }
}

std::list<Notebook::Page>::iterator Notebook::getIterator(unsigned int i) {
    auto it = pages.begin();
    for (unsigned int j = 0; j < i; ++j) {
        if (it != pages.end()) { ++it; }
        else {
            break;
        }
    }
    return it;
}

sf::FloatRect Notebook::contentArea() const {
    return {getAcquisition().left + 2,
            getAcquisition().top + tabArea->getRequisition().y + 2,
            getAcquisition().width - 4,
            getAcquisition().height - tabArea->getRequisition().y - 4};
}

} // namespace gui
} // namespace bl
