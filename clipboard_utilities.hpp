#include "panels.h"
#include "history_cleaner.h"
#include "widgets.h"
#include "utils.h"
#include "file_operating.h"
#include "dataobjects.h"
#include "clipboard_operating.h"

class CustomLogTarget : public wxLog {
public:
    void DoLogRecord(wxLogLevel level, const wxString &msg, const wxLogRecordInfo &info) override;
};

class CustomFrame;

class CustomTeskBarIcon: public wxTaskBarIcon {
    CustomFrame *frame;
public:
    CustomTeskBarIcon(CustomFrame *frame);
    template <class _Evt>
    void OnEventIconToFrame(_Evt&);
    void OnRightButtonClick(wxTaskBarIconEvent&);
    void onEventExit(wxCommandEvent&);
    wxMenu *CreatePopupMenu() override;
    wxDECLARE_EVENT_TABLE();
};

class CustomFrame: public wxFrame {
    CustomTeskBarIcon taskBarIcon;
    wxNotebook notebook;
    wxPanel mainPanel;
    wxTextCtrl logTextCtrl;
    IntroPanel introPanel;
    wxScrolledWindow *configWindow;
public:
    CustomFrame();
    std::string clipboardFunc();

    wxTextCtrl& getLogTextCtrl() {
        return this->logTextCtrl;
    }

    void OnClose(wxCloseEvent&);
    wxDECLARE_EVENT_TABLE();
};


class CustomApp: public wxApp {
    CustomFrame *customFrame;
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

// =====================================
//              definition
// =====================================

template <class _Evt>
void CustomTeskBarIcon::OnEventIconToFrame(_Evt&) {
    this->frame->Show(true);
    this->frame->Iconize(false);
}
