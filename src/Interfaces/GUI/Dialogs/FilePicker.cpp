#include <BLIB/Interfaces/GUI.hpp>

#include <BLIB/Files/Util.hpp>

namespace bl
{
namespace gui
{
namespace
{
sf::Clock timer;
constexpr float DoubleClick = 1.25f;
} // namespace

FilePicker::FilePicker(const std::string& rootdir, const std::vector<std::string>& extensions,
                       const ChooseCb& onChoose, const CancelCb& onCancel)
: root(rootdir)
, extensions(extensions)
, onChoose(onChoose) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 0), "File picker");
    window->setRequisition({500, 600});
    window->getSignal(Action::Closed).willAlwaysCall([onCancel](const Action&, Element*) {
        onCancel();
    });

    Box::Ptr pathRow      = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr pathReset = Button::create(file::Util::joinPath(root, " "));
    pathBox               = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    pathReset->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
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
    chooseButton->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        onChooseClicked();
    });
    Button::Ptr cancelButton = Button::create("Cancel");
    cancelButton->getSignal(Action::LeftClicked)
        .willAlwaysCall([onCancel](const Action&, Element*) { onCancel(); });
    entryRow->pack(Label::create("File:"), false, true);
    entryRow->pack(fileEntry, true, true);
    entryRow->pack(chooseButton, false, true);
    entryRow->pack(cancelButton, false, true);
    window->pack(entryRow, true, false);
}

void FilePicker::open(Mode m, const std::string& title, GUI::Ptr parent, bool rpath) {
    mode = m;
    window->getTitleLabel()->setText(title);

    if (rpath) {
        path.clear();
        for (auto& but : pathButtons) { but->remove(); }
        pathButtons.clear();
    }

    populateFiles();
    parent->pack(window);
}

void FilePicker::close() { window->remove(); }

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

    if (f != clickedFile) {
        clickedFile   = f;
        fileClickTime = timer.getElapsedTime().asSeconds();
    }
    else {
        const float n = timer.getElapsedTime().asSeconds();
        if (n - fileClickTime < DoubleClick) {
            path.emplace_back(f);
            populateFiles();
        }
        else {
            fileClickTime = n;
        }
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
        if (n - fileClickTime < DoubleClick) {
            // TODO - check mode and confirm overwrite
            onChooseClicked();
        }
        else {
            fileClickTime = n;
        }
    }
}

void FilePicker::onChooseClicked() {
    onChoose(file::Util::joinPath(buildPath(), fileEntry->getInput()));
}

void FilePicker::highlight(const std::string& f) {
    for (auto& pair : fileLabels) {
        if (pair.first == f) { pair.second->setColor(sf::Color(20, 75, 240), sf::Color::Blue); }
        else {
            pair.second->setColor(sf::Color::Transparent, sf::Color::Transparent);
        }
    }
}

void FilePicker::populateFiles() {
    for (auto& pair : fileLabels) { pair.second->remove(); }
    fileLabels.clear();
    fileEntry->setInput("");
    clickedFile   = "";
    fileClickTime = 0.f;

    const std::string p = file::Util::joinPath(root, buildPath());
    std::vector<std::string> files, folders;
    for (const std::string& ext : extensions) {
        std::vector f = file::Util::listDirectory(p, ext, false);
        files.insert(
            files.end(), std::make_move_iterator(f.begin()), std::make_move_iterator(f.end()));
    }
    folders = file::Util::listDirectoryFolders(p);
    for (std::string& f : files) { f = file::Util::getFilename(f); }
    std::sort(files.begin(), files.end());
    std::sort(folders.begin(), folders.end());

    for (const std::string& f : folders) {
        const auto onClick = [this, f](const Action&, Element*) { onFolderClick(f); };

        Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
        row->getSignal(Action::LeftClicked).willAlwaysCall(onClick);
        Label::Ptr label = Label::create("Folder:");
        label->getSignal(Action::LeftClicked).willAlwaysCall(onClick);
        row->pack(label, false, true);
        label = Label::create(f);
        label->setHorizontalAlignment(RenderSettings::Left);
        label->getSignal(Action::LeftClicked).willAlwaysCall(onClick);
        row->getSignal(Action::LeftClicked).willAlwaysCall(onClick);
        row->setRequisition({0, 20});
        row->pack(label, true, true);
        filesBox->pack(row, true, false);
        fileLabels[f] = row;
    }

    for (const std::string& f : files) {
        const auto onClick = [this, f](const Action&, Element*) { onFileClick(f); };

        Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 6));
        row->getSignal(Action::LeftClicked).willAlwaysCall(onClick);
        Label::Ptr label = Label::create("File:");
        label->getSignal(Action::LeftClicked).willAlwaysCall(onClick);
        row->pack(label, false, true);
        label = Label::create(f);
        label->setHorizontalAlignment(RenderSettings::Left);
        label->getSignal(Action::LeftClicked).willAlwaysCall(onClick);
        row->getSignal(Action::LeftClicked).willAlwaysCall(onClick);
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
        but->getSignal(Action::LeftClicked).willAlwaysCall([this, i](const Action&, Element*) {
            onPathClick(i);
        });
        b->pack(Label::create("->"));
        b->pack(but);
        pathButtons.push_back(b);
        pathBox->pack(b);
    }

    filesScroll->setScroll({0.f, 0.f});
    filesScroll->makeDirty();
}

std::string FilePicker::buildPath() const {
    std::string result;
    for (const std::string& p : path) { result = file::Util::joinPath(result, p); }
    return result;
}

} // namespace gui
} // namespace bl
