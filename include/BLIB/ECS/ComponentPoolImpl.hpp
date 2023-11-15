#ifndef BLIB_ECS_COMPONENTPOOLIMPL_HPP
#define BLIB_ECS_COMPONENTPOOLIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/Traits/ChildAware.hpp>
#include <BLIB/ECS/Traits/IgnoresDummy.hpp>
#include <BLIB/ECS/Traits/ParentAware.hpp>
#include <type_traits>
#include <vector>

namespace bl
{
namespace ecs
{
template<typename T>
void ComponentPool<T>::setParent(Entity child, Entity parent) {
    if constexpr (std::is_base_of_v<trait::IgnoresDummy, T>) {
        // need to connect all children of child if child is dummy
        if (child.flagSet(Flags::Dummy)) {
            for (Entity grandchild : owner.getEntityChildren(child)) {
                setParent(grandchild, parent);
            }
            return;
        }

        // otherwise find first non-dummy parent before setting
        while (parent.flagSet(Flags::Dummy)) {
            parent = owner.getEntityParent(parent);
            if (parent == InvalidEntity) { return; }
        }
    }

    T* childCom  = get(child);
    T* parentCom = get(parent);

    if (childCom && parentCom) { childCom->parent = parentCom; }
    else {
        if constexpr (!std::is_base_of_v<trait::IgnoresDummy, T>) {
            if (!childCom) { BL_LOG_ERROR << "Invalid child entity: " << child; }
            else { BL_LOG_ERROR << "Invalid parent entity: " << parent; }
        }
    }
}

template<typename T>
void ComponentPool<T>::addChild(Entity child, Entity parent) {
    // find first non-dummy parent
    if constexpr (std::is_base_of_v<trait::IgnoresDummy, T>) {
        while (parent.flagSet(Flags::Dummy)) {
            parent = owner.getEntityParent(parent);
            if (parent == InvalidEntity) { return; }
        }
    }

    T* parentCom = get(parent);
    if (!parentCom) { return; }

    const auto addSingleChild = [this, parentCom](Entity child) {
        T* childCom = get(child);

        if (childCom) { parentCom->children.emplace_back(childCom); }
    };

    if constexpr (std::is_base_of_v<trait::IgnoresDummy, T>) {
        if (!child.flagSet(Flags::Dummy)) { addSingleChild(child); }
        else {
            std::vector<Entity> childStack;
            childStack.reserve(16);
            childStack.emplace_back(child);

            while (!childStack.empty()) {
                const Entity ent = childStack.back();
                childStack.pop_back();

                for (Entity grandchild : owner.getEntityChildren(ent)) {
                    if (grandchild.flagSet(Flags::Dummy)) { childStack.emplace_back(grandchild); }
                    else { addSingleChild(grandchild); }
                }
            }
        }
    }
    else { addSingleChild(child); }
}

template<typename T>
void ComponentPool<T>::removeParent(Entity orphan) {
    T* com = get(orphan);
    if (com) { com->parent = nullptr; }
    else { BL_LOG_WARN << "Invalid orphan entity: " << orphan; }

    // if orphan is a dummy we also need to remove the parent of all non-dummy children
    if constexpr (std::is_base_of_v<trait::IgnoresDummy, T>) {
        if (orphan.flagSet(Flags::Dummy)) {
            std::vector<Entity> childStack;
            childStack.reserve(16);
            childStack.emplace_back(orphan);

            while (!childStack.empty()) {
                const Entity ent = childStack.back();
                childStack.pop_back();

                for (Entity grandchild : owner.getEntityChildren(ent)) {
                    if (grandchild.flagSet(Flags::Dummy)) { childStack.emplace_back(grandchild); }
                    else {
                        T* com = get(grandchild);
                        if (com) { com->parent = nullptr; }
                    }
                }
            }
        }
    }
}

template<typename T>
void ComponentPool<T>::removeChild(Entity parent, Entity orphan) {
    // find first non-dummy parent
    if constexpr (std::is_base_of_v<trait::IgnoresDummy, T>) {
        while (parent.flagSet(Flags::Dummy)) {
            parent = owner.getEntityParent(parent);
            if (parent == InvalidEntity) { return; }
        }
    }

    T* pcom = get(parent);
    if (!pcom) {
        BL_LOG_ERROR << "Invalid parent entity: " << parent;
        return;
    }

    const auto removeSingleChild = [this, pcom](Entity orphan) {
        T* com = get(orphan);

        if (com) {
            auto& c = pcom->children;
            auto it = std::find(c.begin(), c.end(), com);
            if (it != c.end()) { c.erase(it); }
        }
    };

    // if child is a dummy then we need to get to first non-dummy children and remove those
    if constexpr (std::is_base_of_v<trait::IgnoresDummy, T>) {
        if (orphan.flagSet(Flags::Dummy)) {
            std::vector<Entity> childStack;
            childStack.reserve(16);
            childStack.emplace_back(orphan);

            while (!childStack.empty()) {
                const Entity ent = childStack.back();
                childStack.pop_back();

                for (Entity grandchild : owner.getEntityChildren(ent)) {
                    if (grandchild.flagSet(Flags::Dummy)) { childStack.emplace_back(grandchild); }
                    else {
                        T* com = get(grandchild);
                        if (com) { com->parent = nullptr; }
                    }
                }
            }
        }
        else { removeSingleChild(orphan); }
    }
    else { removeSingleChild(orphan); }
}

} // namespace ecs
} // namespace bl

#endif
