#include <BLIB/Interfaces/Menu.hpp>

namespace bl
{
namespace menu
{
SubmenuItem::Ptr SubmenuItem::create(Menu& parent, const Item::Ptr& i, AttachPoint sp,
                                     AttachPoint md) {
    return Ptr{new SubmenuItem(parent, i, sp, md)};
}

SubmenuItem::SubmenuItem(Menu& parent, const Item::Ptr& i, AttachPoint sp, AttachPoint md)
: openDir(sp)
, menuDir(md)
, parent(parent)
, self(i) {
    options.reserve(8);
    getSignal(Activated).willAlwaysCall([this]() { openMenu(); });
}

void SubmenuItem::addOption(const Item::Ptr& opt, bool isBack) {
    options.emplace_back(opt);
    if (isBack) {
        opt->getSignal(Activated).willAlwaysCall([this]() { closeMenu(); });
    }
}

void SubmenuItem::openMenu() {
    if (!open && !options.empty()) {
        open = true;

        parent.addItem(options.front(), this, openDir, false);
        Item* prev = options.front().get();
        for (unsigned int i = 1; i < options.size(); ++i) {
            parent.addItem(options[i], prev, menuDir);
            prev = options[i].get();
        }
        parent.setSelectedItem(options.front().get());
    }
}

void SubmenuItem::closeMenu() {
    if (open) {
        open = false;
        parent.setSelectedItem(this);
        for (auto& opt : options) { parent.removeItem(opt.get(), false); }
    }
}

constexpr bool SubmenuItem::isOpen() const { return open; }

sf::Vector2f SubmenuItem::getSize() const { return self->getSize(); }

void SubmenuItem::render(sf::RenderTarget& target, sf::RenderStates states,
                         const sf::Vector2f& position) const {
    self->render(target, states, position);
}

} // namespace menu
} // namespace bl
