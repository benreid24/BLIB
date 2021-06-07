#include <BLIB/Interfaces/GUI/Dialogs/FilePicker.hpp>

namespace bl
{
namespace gui
{
FilePicker::Ptr FilePicker::create(const std::string& rootdir,
                                   const std::vector<std::string>& extensions,
                                   const ChooseCb& onChoose, const CancelCb& onCancel) {
    return Ptr(new FilePicker(rootdir, extensions, onChoose, onCancel));
}

FilePicker::FilePicker(const std::string& rootdir, const std::vector<std::string>& extensions,
                       const ChooseCb& onChoose, const CancelCb& onCancel)
: root(rootdir)
, extensions(extensions) {
    // TODO
}

} // namespace gui
} // namespace bl
