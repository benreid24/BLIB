#include <BLIB/Assets/Ref.hpp>

#include <BLIB/Assets/RepoAsset.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <stdexcept>

namespace bl
{
namespace as
{
Ref::Ref()
: repo(nullptr)
, asset(nullptr) {}

Ref::Ref(Repository* repo, RepoAsset* asset)
: repo(repo)
, asset(asset) {
    if (asset) { addRef(); }
}

Ref::Ref(const Ref& copy)
: repo(copy.repo)
, asset(copy.asset) {
    if (asset) { addRef(); }
}

Ref::Ref(Ref&& move)
: repo(move.repo)
, asset(move.asset) {
    move.repo  = nullptr;
    move.asset = nullptr;
}

Ref::~Ref() {
    if (asset) { removeRef(); }
}

Ref& Ref::operator=(const Ref& copy) {
    if (this != &copy) {
        if (asset) { removeRef(); }
        repo  = copy.repo;
        asset = copy.asset;
        if (asset) { addRef(); }
    }
    return *this;
}

Ref& Ref::operator=(Ref&& move) {
    if (this != &move) {
        if (asset) { removeRef(); }
        repo       = move.repo;
        asset      = move.asset;
        move.repo  = nullptr;
        move.asset = nullptr;
    }
    return *this;
}

bool Ref::isValid() const { return asset != nullptr; }

Ref::operator bool() const { return isValid(); }

State Ref::getState() const {
    if (!asset) { return State::Unknown; }
    return asset->asset.getState();
}

Asset& Ref::getAsset() {
    if (!asset) {
        BL_LOG_ERROR << "Accessing empty Asset Ref";
        throw std::runtime_error("Accessing empty Asset Ref");
    }
    return asset->asset;
}

const Asset& Ref::getAsset() const {
    if (!asset) {
        BL_LOG_ERROR << "Accessing empty Asset Ref";
        throw std::runtime_error("Accessing empty Asset Ref");
    }
    return asset->asset;
}

Asset* Ref::operator->() { return &getAsset(); }

const Asset* Ref::operator->() const { return &getAsset(); }

void Ref::release() {
    if (asset) {
        removeRef();
        repo  = nullptr;
        asset = nullptr;
    }
}

void Ref::addRef() { ++asset->refCount; }

void Ref::removeRef() {
    if (asset) {
        if (--asset->refCount == 0) { repo->queueUnload(asset->asset.getUUID()); }
        repo  = nullptr;
        asset = nullptr;
    }
}

unsigned int Ref::getCount() const {
    if (!asset) { return 0; }
    return asset->refCount.load();
}

} // namespace as
} // namespace bl
