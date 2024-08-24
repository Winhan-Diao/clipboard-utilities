#include "panels.h"
#include "history_cleaner.h"

std::string getTimeString() {
    auto now = std::chrono::system_clock::now();
    std::stringstream ss{};
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    auto nano = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    ss << std::put_time(std::localtime(&timeT), "%Y-%m-%d-%H-%M-") << nano;
    return ss.str();
}

void bufferToFile(const void *buffer, const size_t totalSize, const char *fileName) {
    // wxFileName f{fileName};
    // std::cout << "fileName " << f.GetPath() << "\r\n";      //debug
    // std::cout << "DirExists " << f.DirExists() << "\r\n";       //debug
    wxFileName::Mkdir(wxFileName{fileName}.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if (totalSize <= 0) return;
    std::FILE *file = std::fopen(fileName, "wb");
    if (file) {
        std::fwrite(buffer, 1, totalSize, file);
        std::fclose(file);
    } else {
        std::cerr << "Failed to write to file" << "\r\n";
    }
    fclose(file);
}

class CustomFrame;

class CustomTeskBarIcon: public wxTaskBarIcon {
    CustomFrame *frame;
public:
    CustomTeskBarIcon(CustomFrame *frame);
    void OnLeftButtonClick(wxTaskBarIconEvent&);

    wxDECLARE_EVENT_TABLE();
};

class CustomFrame: public wxFrame {
    CustomTeskBarIcon taskBarIcon;
    wxNotebook notebook;
    wxPanel mainPanel;
    wxTextCtrl logTextCtrl;
    IntroPanel introPanel;
    ConfigPanel configPanel;
public:
    CustomFrame();
    std::string clipboardFunc();

    wxTextCtrl& getLogTextCtrl() {
        return this->logTextCtrl;
    }
};


class CustomApp: public wxApp {
    std::unique_ptr<CustomFrame> customFrame;
    bool OnInit() override;
    int OnExit() override;
public:
    CustomApp();
    wxFrame *getFrame();
};

bool removeOldest(const wxString& directory);
wxString getOldest(const wxString& directory);
bool checkIfToClean(const wxString& directory);

DECLARE_APP(CustomApp)
