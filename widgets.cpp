#include "widgets.h"

namespace builder {
    wxPanel *buildVStaticBoxPanel(wxWindow *parent, const wxString& title, const wxSizerFlags& sizerFlags) {
        wxPanel *panel = new wxPanel{parent};
        wxStaticBox *configBox = new wxStaticBox{panel, wxID_ANY, title};
        wxStaticBoxSizer *configBoxSizer = new wxStaticBoxSizer{configBox, wxVERTICAL};
        panel->SetSizer(configBoxSizer);
        parent->GetSizer()->Add(panel, sizerFlags);
        return panel;
    }

}