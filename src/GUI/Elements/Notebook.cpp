#include <BLIB/GUI/Elements/Notebook.hpp>

#include <BLIB/GUI/Packers/LinePacker.hpp>

namespace bl
{
namespace gui
{
Notebook::Ptr Notebook::create(const std::string& group, const std::string& id) {
    Ptr nb(new Notebook(group, id));
    nb->addChildren();
    return nb;
}

Notebook::Notebook(const std::string& group, const std::string& id)
: Container(group, id)
, tabArea(Box::create(LinePacker::create(), group, id + "-tabarea"))
, activePage(0) {
    // todo ?
}

void Notebook::addChildren() { add(tabArea); }

Notebook::~Notebook() {
    while (!pages.empty()) removePageByIndex(0);
}

Notebook::Page::Page(const std::string& name, Label::Ptr label, Element::Ptr content)
: name(name)
, label(label)
, content(content) {}

void Notebook::addPage(const std::string& name, const std::string& title,
                       Element::Ptr content) {
    if (pageMap.find(name) == pageMap.end()) {
        pages.push_back(
            new Page(name, Label::create(title, group(), id() + "-tab-" + name), content));
        pageMap[name] = std::make_pair(pages.size() - 1, pages.back());

        tabArea->pack(pages.back()->label);
        pages.back()
            ->label->getSignal(Action::LeftClicked)
            .willAlwaysCall(std::bind(&Notebook::pageClicked, this, pages.back()));
        if (pages.size() == 1) makePageActive(0);
    }
}

unsigned int Notebook::getActivePage() const { return activePage; }

unsigned int Notebook::pageCount() const { return pages.size(); }

const std::vector<Notebook::Page*>& Notebook::getPages() const { return pages; }

const std::string& Notebook::getActivePageName() const {
    static const std::string empty;
    if (activePage < pages.size()) return pages[activePage]->name;
    return empty;
}

Notebook::Page* Notebook::getPageByIndex(unsigned int i) {
    if (i < pages.size()) return pages[i];
    return nullptr;
}

Notebook::Page* Notebook::getPageByName(const std::string& name) {
    auto i = pageMap.find(name);
    if (i != pageMap.end()) return i->second.second;
    return nullptr;
}

void Notebook::removePageByIndex(unsigned int i) {
    if (i < pages.size()) {
        pages[i]->label->remove();
        pages[i]->content->remove();
        pageMap.erase(pages[i]->name);
        if (i > 0) makePageActive(i - 1);
        delete pages[i];
        pages.erase(pages.begin() + i);
    }
}

void Notebook::removePageByName(const std::string& name) {
    auto i = pageMap.find(name);
    if (i != pageMap.end()) removePageByIndex(i->second.first);
}

sf::Vector2i Notebook::minimumRequisition() const {
    const sf::Vector2i tabReq = tabArea->getRequisition();
    sf::Vector2i contentReq(0, 0);
    for (unsigned int i = 0; i < pages.size(); ++i) {
        const sf::Vector2i creq = pages[i]->content->getRequisition();
        contentReq.x            = std::max(contentReq.x, creq.x);
        contentReq.y            = std::max(contentReq.y, creq.y);
    }
    return {std::max(tabReq.x, contentReq.x), tabReq.y + contentReq.y};
}

void Notebook::onAcquisition() {
    Packer::manuallyPackElement(tabArea,
                                {0, 0, getAcquisition().width, tabArea->getRequisition().y});
    contentArea = {0,
                   tabArea->getRequisition().y,
                   getAcquisition().width,
                   getAcquisition().height - tabArea->getRequisition().y};
    if (activePage < pages.size())
        Packer::manuallyPackElement(pages[activePage]->content, contentArea);
}

void Notebook::doRender(sf::RenderTarget& target, sf::RenderStates states,
                        Renderer::Ptr renderer) const {
    const sf::View oldView = target.getView();
    target.setView(computeView(target, states.transform));
    renderer->renderNotebook(target, states, *this);
    target.setView(oldView);
}

void Notebook::pageClicked(Page* page) {
    auto i = pageMap.find(page->name);
    if (i != pageMap.end()) makePageActive(i->second.first);
}

void Notebook::makePageActive(unsigned int i) {
    if (i < pages.size()) {
        if (activePage < pages.size()) pages[activePage]->content->remove();
        add(pages[i]->content);
        Packer::manuallyPackElement(pages[i]->content, contentArea);
        activePage = i;
    }
}

} // namespace gui
} // namespace bl