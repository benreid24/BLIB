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
    using ChooseCb = std::function<void(const std::string&)>;
    using CancelCb = std::function<void()>;

    enum Mode { CreateNew, PickExisting };

    FilePicker(const std::string& rootdir, const std::vector<std::string>& extensions,
               const ChooseCb& onChoose, const CancelCb& onCancel);

    void open(Mode mode, const std::string& title, GUI::Ptr parent, bool resetPath = false);

    void close();

private:
    Mode mode;
    const std::string root;
    const std::vector<std::string> extensions;
    const ChooseCb onChoose;
    std::vector<std::string> path;

    Window::Ptr window;
    sf::RenderTexture folderTexture;

    Box::Ptr pathBox;
    std::vector<Box::Ptr> pathButtons;

    Box::Ptr filesBox;
    ScrollArea::Ptr filesScroll;
    std::unordered_map<std::string, Box::Ptr> fileLabels;
    TextEntry::Ptr fileEntry;

    std::string clickedFile;
    float fileClickTime;

    void onClearPath();
    void onPathClick(unsigned int i);
    void onFolderClick(const std::string& folder);
    void onFileClick(const std::string& file);
    void onChooseClicked();

    void highlight(const std::string& file);
    void populateFiles();
    std::string buildPath() const;
};

} // namespace gui
} // namespace bl

#endif
