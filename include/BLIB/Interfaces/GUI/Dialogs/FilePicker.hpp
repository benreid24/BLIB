#ifndef BLIB_INTERFACES_GUI_DIALOGS_FILEPICKER_HPP
#define BLIB_INTERFACES_GUI_DIALOGS_FILEPICKER_HPP

#include <BLIB/Interfaces/GUI/Elements/TextEntry.hpp>
#include <BLIB/Interfaces/GUI/Elements/Window.hpp>
#include <BLIB/Interfaces/GUI/GUI.hpp>

namespace bl
{
namespace gui
{
class FilePicker {
public:
    typedef std::shared_ptr<FilePicker> Ptr;

    using ChooseCb = std::function<void(const std::string&)>;
    using CancelCb = std::function<void()>;

    enum Mode { CreateNew, PickExisting };

    static Ptr create(const std::string& rootdir, const std::vector<std::string>& extensions,
                      const ChooseCb& onChoose, const CancelCb& onCancel);

    void show(Mode mode, const std::string& title, GUI::Ptr parent, bool resetPath = false);

    void hide();

private:
    Mode mode;
    const std::string root;
    const std::vector<std::string> extensions;
    std::vector<std::string> path;

    Window::Ptr window;

    Label::Ptr pathLabel;
    Box::Ptr pathBox;
    std::vector<Button::Ptr> pathButtons;

    Box::Ptr filesBox;
    std::unordered_map<std::string, Box::Ptr> fileLabels;
    TextEntry::Ptr fileEntry;

    std::string clickedFile;
    float fileClickTime;

    FilePicker(const std::string& rootdir, const std::vector<std::string>& extensions,
               const ChooseCb& onChoose, const CancelCb& onCancel);

    void onPathClick(unsigned int i);
    void onFolderClick(const std::string& folder);
    void onFileClick(const std::string& file);
    void onChooseClicked();

    void highlight(const std::string& file);
    void populateFiles();
};

} // namespace gui
} // namespace bl

#endif
