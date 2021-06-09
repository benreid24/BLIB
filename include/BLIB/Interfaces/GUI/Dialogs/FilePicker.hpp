#ifndef BLIB_INTERFACES_GUI_DIALOGS_FILEPICKER_HPP
#define BLIB_INTERFACES_GUI_DIALOGS_FILEPICKER_HPP

#include <BLIB/Interfaces/GUI/Elements/TextEntry.hpp>
#include <BLIB/Interfaces/GUI/Elements/Window.hpp>
#include <BLIB/Interfaces/GUI/GUI.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Helper class for picking files limited to subdirectories of the current directory. For
 *        other uses use the tinyfd functions defined in the dialog namespace
 *
 * @ingroup GUI
 */
class FilePicker {
public:
    using ChooseCb = std::function<void(const std::string&)>;
    using CancelCb = std::function<void()>;

    /// What mode to pick file in
    enum Mode { CreateNew, PickExisting };

    /**
     * @brief Construct a new File Picker dialog
     *
     * @param rootdir The starting directory. The picker will not go above this
     * @param extensions List of extenions to allow
     * @param onChoose Callback for when a file is selected
     * @param onCancel Callback for when the window is closed without a selection
     */
    FilePicker(const std::string& rootdir, const std::vector<std::string>& extensions,
               const ChooseCb& onChoose, const CancelCb& onCancel);

    /**
     * @brief Opens the dialog in the given mode with the given settings
     *
     * @param mode The mode to open in
     * @param title The title of the window
     * @param parent The main GUI object to attach to
     * @param resetPath True to reset the starting path to root dir, false to keep the same
     */
    void open(Mode mode, const std::string& title, GUI::Ptr parent, bool resetPath = false);

    /**
     * @brief Closes the dialog
     *
     */
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
