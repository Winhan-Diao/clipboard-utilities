#pragma once
#include "usage.h"

namespace builder {
    template <class P, class... Types>
    wxScrolledWindow *buildSingleChildSrcolledWindow(wxWindow *parent, Types... args) {
        wxScrolledWindow *sw = new wxScrolledWindow{parent};
        sw->SetScrollbars(0, 1, 0, 50);
        wxBoxSizer *bs = new wxBoxSizer(wxVERTICAL);
        sw->SetSizer(bs);
        bs->Add(new P{sw, args...}, wxSizerFlags().Expand().Proportion(10));

        sw->Hide();
        return sw;
    }

    wxPanel *buildVStaticBoxPanel(wxWindow *parent, const wxString& title, const wxSizerFlags& sizerFlags = wxSizerFlags().Expand().Proportion(0));
}