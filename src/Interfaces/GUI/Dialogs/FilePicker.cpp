#include <BLIB/Interfaces/GUI.hpp>

#include <BLIB/Interfaces/GUI/Dialogs/tinyfiledialogs.hpp>
#include <BLIB/Util/FileUtil.hpp>

namespace bl
{
namespace gui
{
namespace
{
sf::Clock timer;
constexpr float DoubleClick = 1.25f;

constexpr float IconSize = 25.f;
const sf::Color FolderColor(40, 40, 240);
} // namespace

FilePicker::FilePicker(const std::string& rootdir, const std::vector<std::string>& extensions,
                       const ChooseCb& onChoose, const CancelCb& onCancel)
: root(rootdir)
, extensions(extensions)
, onChoose(onChoose) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4), "File picker");
    window->setRequisition({500, 600});
    window->getSignal(Event::Closed).willAlwaysCall([onCancel](const Event&, Element*) {
        onCancel();
    });

    Box::Ptr controlRow = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    Button::Ptr upBut   = Button::create("Up");
    upBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (!path.empty()) {
            path.pop_back();
            populateFiles();
        }
    });
    Button::Ptr makeBut = Button::create("New Folder");
    makeBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        char* name = dialog::tinyfd_inputBox("New Folder", "Folder name:", "");
        if (name) {
            const std::string f    = util::FileUtil::joinPath(buildPath(), name);
            const std::string full = util::FileUtil::joinPath(root, f);
            util::FileUtil::createDirectory(full);
            populateFiles();
        }
    });
    Button::Ptr delBut = Button::create("Delete");
    delBut->setColor(sf::Color::Red, sf::Color::Black);
    delBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (!fileEntry->getInput().empty()) {
            const std::string f    = util::FileUtil::joinPath(buildPath(), fileEntry->getInput());
            const std::string full = util::FileUtil::joinPath(root, f);
            if (util::FileUtil::exists(full)) {
                if (dialog::tinyfd_messageBox("Delete File?",
                                              std::string("Delete file `" + full + "`?").c_str(),
                                              "yesno",
                                              "question",
                                              0) == 1) {
                    util::FileUtil::deleteFile(full);
                    populateFiles();
                }
            }
            else if (util::FileUtil::directoryExists(full)) {
                if (dialog::tinyfd_messageBox("Delete Folder?",
                                              std::string("Delete folder `" + full + "`?").c_str(),
                                              "yesno",
                                              "question",
                                              0) == 1) {
                    util::FileUtil::deleteDirectory(full);
                    populateFiles();
                }
            }
        }
    });
    controlRow->pack(upBut, false, true);
    controlRow->pack(makeBut, false, true);
    controlRow->pack(delBut, false, true);
    window->pack(controlRow, true, false);

    Box::Ptr pathRow      = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr pathReset = Button::create(util::FileUtil::joinPath(root, " "));
    pathBox               = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    pathReset->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        onClearPath();
    });
    pathRow->pack(pathReset, false, true);
    pathRow->pack(pathBox, true, true);
    window->pack(pathRow, true, false);

    filesScroll = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4));
    filesBox    = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    filesScroll->setMaxSize({500, 400});
    filesBox->setColor(sf::Color::White, sf::Color::Black);
    filesBox->setOutlineThickness(2);
    filesScroll->pack(filesBox, true, true);
    window->pack(filesScroll, true, true);

    Box::Ptr entryRow        = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
    fileEntry                = TextEntry::create(1);
    Button::Ptr chooseButton = Button::create("Select");
    chooseButton->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        onChooseClicked();
    });
    Button::Ptr cancelButton = Button::create("Cancel");
    cancelButton->getSignal(Event::LeftClicked).willAlwaysCall([onCancel](const Event&, Element*) {
        onCancel();
    });
    entryRow->pack(Label::create("File:"), false, true);
    entryRow->pack(fileEntry, true, true);
    entryRow->pack(chooseButton, false, true);
    entryRow->pack(cancelButton, false, true);
    window->pack(entryRow, true, false);
}

void FilePicker::open(Mode m, const std::string& title, GUI* parent, bool rpath) {
    mode = m;
    window->getTitleLabel()->setText(title);

    if (rpath) {
        path.clear();
        for (auto& but : pathButtons) { but->remove(); }
        pathButtons.clear();
    }

    populateFiles();
    parent->pack(window);
    window->setForceFocus(true);
}

void FilePicker::close() {
    window->remove();
    window->setForceFocus(false);
}

void FilePicker::onClearPath() {
    if (!path.empty()) {
        path.clear();
        populateFiles();
    }
}

void FilePicker::onPathClick(unsigned int i) {
    if (i < path.size() - 1) {
        path.erase(path.begin() + i + 1, path.end());
        populateFiles();
    }
}

void FilePicker::onFolderClick(const std::string& f) {
    highlight(f);
    fileEntry->setInput(f);

    if (f != clickedFile) {
        clickedFile   = f;
        fileClickTime = timer.getElapsedTime().asSeconds();
    }
    else {
        const float n = timer.getElapsedTime().asSeconds();
        if (n - fileClickTime < DoubleClick) {
            path.emplace_back(f);
            populateFiles();
            fileEntry->setInput("");
        }
        else { fileClickTime = n; }
    }
}

void FilePicker::onFileClick(const std::string& f) {
    highlight(f);
    fileEntry->setInput(f);

    if (f != clickedFile) {
        clickedFile   = f;
        fileClickTime = timer.getElapsedTime().asSeconds();
    }
    else {
        const float n = timer.getElapsedTime().asSeconds();
        if (n - fileClickTime < DoubleClick) { onChooseClicked(); }
        else { fileClickTime = n; }
    }
}

void FilePicker::onChooseClicked() {
    if (fileEntry->getInput().empty()) {
        dialog::tinyfd_messageBox("Error", "You must enter a filename", "ok", "error", 1);
        return;
    }

    const std::string localpath = util::FileUtil::joinPath(buildPath(), fileEntry->getInput());
    const std::string filename  = util::FileUtil::joinPath(root, localpath);

    if (util::FileUtil::directoryExists(filename)) {
        fileClickTime = timer.getElapsedTime().asSeconds(); // trick
        onFolderClick(fileEntry->getInput());
        return;
    }

    if (mode == CreateNew) {
        if (util::FileUtil::exists(filename)) {
            if (dialog::tinyfd_messageBox("Overwrite file?",
                                          (filename + " already exists, overwrite?").c_str(),
                                          "yesno",
                                          "warning",
                                          0) != 1) {
                return;
            }
        }
        onChoose(localpath);
    }
    else {
        if (mode == CreateOrPick || util::FileUtil::exists(filename)) { onChoose(localpath); }
        else {
            dialog::tinyfd_messageBox(
                "Error", (fileEntry->getInput() + " does not exist").c_str(), "ok", "error", 1);
        }
    }
}

void FilePicker::highlight(const std::string& f) {
    for (auto& pair : fileLabels) {
        if (pair.first == f) { pair.second->setColor(sf::Color(40, 95, 250), sf::Color::Blue); }
        else { pair.second->setColor(sf::Color::Transparent, sf::Color::Transparent); }
    }
}

void FilePicker::populateFiles() {
    for (auto& pair : fileLabels) { pair.second->remove(); }
    fileLabels.clear();
    fileEntry->setInput("");
    clickedFile   = "";
    fileClickTime = 0.f;

    const std::string p = util::FileUtil::joinPath(root, buildPath());
    std::vector<std::string> files, folders;
    for (const std::string& ext : extensions) {
        std::vector<std::string> f = util::FileUtil::listDirectory(p, ext, false);
        files.insert(
            files.end(), std::make_move_iterator(f.begin()), std::make_move_iterator(f.end()));
    }
    folders = util::FileUtil::listDirectoryFolders(p);
    for (std::string& f : files) { f = util::FileUtil::getFilename(f); }
    std::sort(files.begin(), files.end());
    std::sort(folders.begin(), folders.end());

    for (const std::string& f : folders) {
        const auto onClick = [this, f](const Event&, Element*) { onFolderClick(f); };

        Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
        row->getSignal(Event::LeftClicked).willAlwaysCall(onClick);

        Icon::Ptr icon = Icon::create(Icon::Type::Folder, {IconSize, IconSize});
        icon->setColor(FolderColor, sf::Color::Transparent);
        icon->getSignal(Event::LeftClicked).willAlwaysCall(onClick);
        row->pack(icon, false, true);

        Label::Ptr label = Label::create(f);
        label->setHorizontalAlignment(RenderSettings::Left);
        label->getSignal(Event::LeftClicked).willAlwaysCall(onClick);
        row->getSignal(Event::LeftClicked).willAlwaysCall(onClick);
        row->setRequisition({0, 20});
        row->pack(label, true, true);
        filesBox->pack(row, true, false);
        fileLabels[f] = row;
    }

    for (const std::string& f : files) {
        const auto onClick = [this, f](const Event&, Element*) { onFileClick(f); };

        Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
        row->getSignal(Event::LeftClicked).willAlwaysCall(onClick);
        Label::Ptr label = Label::create(f);
        label->setHorizontalAlignment(RenderSettings::Left);
        label->getSignal(Event::LeftClicked).willAlwaysCall(onClick);
        row->getSignal(Event::LeftClicked).willAlwaysCall(onClick);
        row->setRequisition({0, 25});
        row->pack(label, true, true);
        filesBox->pack(row, true, false);
        fileLabels[f] = row;
    }

    for (Box::Ptr but : pathButtons) { but->remove(); }
    pathButtons.clear();

    for (unsigned int i = 0; i < path.size(); ++i) {
        Box::Ptr b      = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
        Button::Ptr but = Button::create(path[i]);
        but->getSignal(Event::LeftClicked).willAlwaysCall([this, i](const Event&, Element*) {
            onPathClick(i);
        });
        b->pack(Label::create(" / "), false, true);
        b->pack(but, false, true);
        pathButtons.push_back(b);
        pathBox->pack(b, false, true);
    }

    filesScroll->setScroll({0.f, 0.f});
    filesScroll->makeDirty();
}

std::string FilePicker::buildPath() const {
    std::string result;
    for (const std::string& p : path) { result = util::FileUtil::joinPath(result, p); }
    return result;
}

} // namespace gui
} // namespace bl
