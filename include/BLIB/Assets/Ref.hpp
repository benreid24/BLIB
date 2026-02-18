#ifndef BLIB_ASSETS_REF_HPP
#define BLIB_ASSETS_REF_HPP

#include <BLIB/Assets/Asset.hpp>

namespace bl
{
namespace as
{
class Ref {
public:
    Ref();

    Ref(const Ref& copy);

    Ref(Ref&& move);

    ~Ref();

    Ref& operator=(const Ref& copy);

    Ref& operator=(Ref&& move);

    bool isValid() const;

    operator bool() const;

    State getState() const;

    Asset& getAsset();

    const Asset& getAsset() const;

    Asset* operator->();

    const Asset* operator->() const;

    void release();

    unsigned int getCount() const;

private:
    Asset* asset;

    void addRef();
    void removeRef();
};

} // namespace as
} // namespace bl

#endif
