#include "clipboard_utilities.hpp"

wxDEFINE_EVENT(DRAW_CLIPBOARD_EVENT, wxCommandEvent);

CustomTeskBarIcon::CustomTeskBarIcon(CustomFrame *frame): frame(frame) {
    SetIcon(wxIcon(icon_xpm));
}
void CustomTeskBarIcon::OnLeftButtonClick(wxTaskBarIconEvent&) {
    frame->Show(true);
    frame->Iconize(false);
}
wxBEGIN_EVENT_TABLE(CustomTeskBarIcon, wxTaskBarIcon)
    EVT_TASKBAR_LEFT_UP(CustomTeskBarIcon::OnLeftButtonClick)
wxEND_EVENT_TABLE()

void CustomLogTarget::DoLogRecord(wxLogLevel level, const wxString &msg, const wxLogRecordInfo &info) {
    if (level == wxLOG_Error) {
        reinterpret_cast<CustomFrame *>(wxGetApp().getFrame())->getLogTextCtrl().AppendText("\r\n"s
                                                                                            + "- - - - - - - - - - - -"s + "\r\n"s
                                                                                            + "[Error]\t"s + msg + "\r\n"s
                                                                                            + "\tFile name: "s + info.filename + "\r\n"s
                                                                                            + "\tAt line: "s + std::to_string(info.line) + "\r\n"s 
                                                                                            + "\tFunction name: "s + info.func + "\r\n"s
                                                                                            + "- - - - - - - - - - - -"s + "\r\n"s
                                                                                            );
    } else if (level == wxLOG_Warning) {
        reinterpret_cast<CustomFrame *>(wxGetApp().getFrame())->getLogTextCtrl().AppendText("\r\n"s 
                                                                                            + "- - - - - - - - - - - -"s + "\r\n"s
                                                                                            + "[Warning]\t"s + msg + "\r\n"s
                                                                                            + "\t\tFile name: "s + info.filename + "\r\n"s
                                                                                            + "\t\tAt line: "s + std::to_string(info.line) + "\r\n"s 
                                                                                            + "\t\tFunction name: "s + info.func + "\r\n"s
                                                                                            + "- - - - - - - - - - - -"s + "\r\n"s
                                                                                            );
    } else if (level == wxLOG_Debug) {
        reinterpret_cast<CustomFrame *>(wxGetApp().getFrame())->getLogTextCtrl().AppendText("\r\n"s 
                                                                                            + "- - - - - - - - - - - -"s + "\r\n"s
                                                                                            + "[Debug]\t"s + msg + "\r\n"s
                                                                                            + "\tFile name: "s + info.filename + "\r\n"s
                                                                                            + "\tAt line: "s + std::to_string(info.line) + "\r\n"s 
                                                                                            + "\tFunction name: "s + info.func + "\r\n"s
                                                                                            + "- - - - - - - - - - - -"s + "\r\n"s
                                                                                            );
    } else {
        wxLog::DoLogRecord(level, msg, info);
    }
}


CustomFrame::CustomFrame()
    : wxFrame{nullptr, wxID_ANY, "Clipboard Utilities"}
    , taskBarIcon{this}
    , notebook{this, wxID_ANY}
    , mainPanel{&notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxSUNKEN_BORDER}
    , logTextCtrl{&notebook, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE} 
    , introPanel{&notebook}
    , configWindow{builder::buildSingleChildSrcolledWindow<ConfigPanel>(&notebook)}
{
    SetIcon(icon_xpm);

    wxBoxSizer generalSizer{wxVERTICAL};
    wxBoxSizer *sizer = new wxBoxSizer{wxVERTICAL};
    wxHyperlinkCtrl *enabledText = new wxHyperlinkCtrl{&mainPanel, wx_usage::ID_JMP_PAGE, _T("✅ Clipboard Recording On"), wxEmptyString};
    wxHyperlinkCtrl *disabledText = new wxHyperlinkCtrl{&mainPanel, wx_usage::ID_JMP_PAGE, _T("❎ Clipboard Recording Off"), wxEmptyString};
    wxHyperlinkCtrl *introText = new wxHyperlinkCtrl{&mainPanel, wx_usage::ID_JMP_PAGE, _T("👋 Introduction"), wxEmptyString};
    wxHyperlinkCtrl *configText = new wxHyperlinkCtrl{&mainPanel, wx_usage::ID_JMP_PAGE, _T("🛠️ Set Configurations"), wxEmptyString};
    wxHyperlinkCtrl *aboutText = new wxHyperlinkCtrl{&mainPanel, wx_usage::ID_JMP_PAGE, _T("ℹ About The Project"), wxEmptyString};
    wxHyperlinkCtrl *revealText = new wxHyperlinkCtrl{&mainPanel, wx_usage::ID_JMP_PAGE, _T("📂 Reveal History Folder"), wxEmptyString};
    wxFont textFont{15, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, true};
    wxFont boldTextFont{15, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, true};
    enabledText->SetFont(boldTextFont);
    disabledText->SetFont(boldTextFont);
    introText->SetFont(textFont);
    configText->SetFont(textFont);
    aboutText->SetFont(textFont);
    revealText->SetFont(textFont);
    wxSizerFlags textFlags{wxSizerFlags().Border(wxLEFT, 5)};
    sizer->Add(enabledText, textFlags);
    sizer->Add(disabledText, textFlags);
    sizer->AddSpacer(5);
    sizer->Add(new wxStaticLine{&mainPanel}, wxSizerFlags{textFlags}.Expand());
    sizer->Add(introText, textFlags);
    sizer->AddSpacer(5);
    sizer->Add(configText, textFlags);
    sizer->AddSpacer(5);
    sizer->Add(aboutText, textFlags);
    sizer->AddSpacer(5);
    sizer->Add(revealText, textFlags);
    sizer->AddSpacer(5);
    mainPanel.SetSizer(sizer);

    disabledText->SetNormalColour(wxColor{0x77, 0x77, 0x77});
    disabledText->Hide();

    generalSizer.Add(&mainPanel, wxSizerFlags().Proportion(10).Expand());
    
    
    wxFont logFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Courier New");
    logTextCtrl.SetFont(logFont);
    logTextCtrl.SetEditable(false);

    notebook.AddPage(&mainPanel, "&General");
    notebook.AddPage(&logTextCtrl, "&Log");
    
    Bind(wxEVT_ICONIZE, [&](wxIconizeEvent& e){
        if (e.IsIconized()) {
            this->Hide();
            this->taskBarIcon.SetIcon(wxICON(sample), "Clipboard Utilities");
            e.Skip();
        }
    });
    Bind(DRAW_CLIPBOARD_EVENT, [&](auto& e) {
        if (!general_usage::ENABLED) return;

        logTextCtrl.AppendText(getTimeString() + "\r\n");
        this->clipboardFunc();

        general_usage::debug("remove start");       //debug
        wxDir dir{"clipboard-history"};
        if (wxDir::Exists(dir.GetName())) {
            general_usage::debug(wxString{"remove step 1 "} << dir.GetName());      //debug
            RemoveOldestDirTraverser traverser{};
            dir.Traverse(traverser);
            general_usage::debug(wxString{"remove step 2 "});     //debug
        }
        general_usage::debug("remove end");       //debug

    });
    introText->Bind(wxEVT_HYPERLINK, [&](auto& e) {
        if (notebook.GetPageCount() > 2) notebook.RemovePage(2);
        notebook.AddPage(&introPanel, "&Introduction");
        notebook.SetSelection(2);
    }, wx_usage::ID_JMP_PAGE);
    configText->Bind(wxEVT_HYPERLINK, [&](auto& e) {
        if (notebook.GetPageCount() > 2) notebook.RemovePage(2);
        notebook.AddPage(configWindow, "&Configurations");
        notebook.SetSelection(2);
    }, wx_usage::ID_JMP_PAGE);
    aboutText->Bind(wxEVT_HYPERLINK, [&](auto& e) {
        constexpr auto infoMsg{"Program made by Winhan Diao.\r\n" \
                         "Free for use and distribution.\r\n" \
                         "version 0.0.1-prealpha\r\n"};
        wxMessageDialog{this, infoMsg, "Clipboard Utilities", wxICON_INFORMATION | wxCENTRE}.ShowModal();
    }, wx_usage::ID_JMP_PAGE);
    revealText->Bind(wxEVT_HYPERLINK, [&](auto& e) {
        wxFileName file{"clipboard-history"};
        if (!wxLaunchDefaultBrowser(wxFileSystem::FileNameToURL(file))) {
            wxMessageBox("Can't open such directory.\r\nmanually check if \"clipboard-history\" exists under the root?", "Error", wxOK | wxICON_ERROR);
            wxLogError("Can't open such directory.\r\nmanually check if \"clipboard-history\" exists under the root?");       //error
        }
    }, wx_usage::ID_JMP_PAGE);
    enabledText->Bind(wxEVT_HYPERLINK, [=](auto& e){
        enabledText->Show(false);
        disabledText->Show(true);
        disabledText->SetVisited(false);
        enabledText->GetContainingSizer()->Layout();
        general_usage::ENABLED = false;
    });
    disabledText->Bind(wxEVT_HYPERLINK, [=](auto& e){
        enabledText->Show(true);
        disabledText->Show(false);
        enabledText->SetVisited(false);
        enabledText->GetContainingSizer()->Layout();
        general_usage::ENABLED = true;
    });
    
}

std::string CustomFrame::clipboardFunc() {
    std::string text{};
    if(wxTheClipboard->Open()) {
        if (wxTheClipboard->IsSupported(wxDF_INVALID)) {
            std::cout << "wxDF_INVALID" << "\r\n";
            logTextCtrl.AppendText("wxDF_INVALID\r\n");
        }
        if (wxTheClipboard->IsSupported(wxDF_METAFILE)) {
            std::cout << "wxDF_METAFILE" << "\r\n";
            
            logTextCtrl.AppendText("wxDF_METAFILE\r\n");
        }
        if (wxTheClipboard->IsSupported(wxDF_DIF)) {
            std::cout << "wxDF_DIF" << " ";
            UniversalDataObject difDataObj{wxDF_DIF};
            if (wxTheClipboard->GetData(difDataObj)) {
                size_t size = difDataObj.GetDataSize();
                std::cout << size << "\r\n";
                char *buffer = new char[size]{};
                difDataObj.GetDataHere(buffer);
                bufferToFile(buffer, size, ("clipboard-history/sheets/dif-"s + getTimeString() + ".dif"s).c_str());
                delete[] buffer;
                logTextCtrl.AppendText(wxString{} << "wxDF_DIF " << size << "\r\n");
            } else {
                std::cout << "\r\n";
            }
        }
        if (wxTheClipboard->IsSupported(wxDF_FILENAME)) {
            std::cout << "wxDF_FILENAME" << "\r\n";
            logTextCtrl.AppendText("wxDF_FILENAME\r\n");
        }
        if (wx_usage::ConfigStruct::ENABLE_UNICODE_TEXT & wx_usage::CONFIG.binaryData && wxTheClipboard->IsSupported(wxDF_UNICODETEXT)) {
            std::cout << "wxDF_UNICODETEXT" << " ";
            wxTextDataObject unicodeDataObj{};
            if (wxTheClipboard->GetData(unicodeDataObj)) {
                size_t size = unicodeDataObj.GetDataSize() + 2;
                std::cout << size << "\r\n";
                char *buffer = new char[size]{};
                new(buffer) int16_t{(int16_t)0xFEFF};
                unicodeDataObj.GetDataHere(buffer + 2);
                bufferToFile(buffer, size, ("clipboard-history/unicode-text/unicode-text-"s + getTimeString() + ".txt"s).c_str());
                delete[] buffer;
                logTextCtrl.AppendText("wxDF_UNICODETEXT "s + std::to_string(size) + "\r\n");
            } else {
                std::cout << "\r\n";
            }
        }
        if (wx_usage::ConfigStruct::ENABLE_DIB & wx_usage::CONFIG.binaryData && (wxTheClipboard->IsSupported(wxDF_DIB) || wxTheClipboard->IsSupported(wxDF_BITMAP))) {
            std::cout << "wxDF_DIB/wxDF_BITMAP" << " ";
            wxBitmapDataObject bitmapDataObj{};
            if (wxTheClipboard->GetData(bitmapDataObj)) {
                size_t size = bitmapDataObj.GetDataSize() + 14;
                std::cout << size << "\r\n";
                wxBitmap bitmap{bitmapDataObj.GetBitmap()};
                if (bitmap.IsOk()) {
                    wxFileName::Mkdir(".\\clipboard-history\\image\\", wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
                    if (wx_usage::CONFIG.binaryData & wx_usage::ConfigStruct::STORE_FORMATTED_IMG) {
                        if (wx_usage::CONFIG.imageType == wx_usage::ICO) {
                            general_usage::debug(wxString{"width "} << bitmap.GetWidth() << "; height" << bitmap.GetHeight());        //debug
                            if (std::max(bitmap.GetWidth(), bitmap.GetHeight()) > 255) {
                                if (bitmap.GetHeight() > bitmap.GetWidth())
                                    wxBitmap::Rescale(bitmap, wxSize{bitmap.GetWidth() * 255 / bitmap.GetHeight(), 255});
                                else
                                    wxBitmap::Rescale(bitmap, wxSize{255, bitmap.GetHeight() * 255 / bitmap.GetWidth()});
                            }
                        }
                        bitmap.SaveFile(".\\clipboard-history\\image\\bitmap-"s + getTimeString() + wx_usage::FORMATTED_IMAGE_TYPE_WITH_INFO.at(wx_usage::CONFIG.imageType).second.extention, wx_usage::FORMATTED_IMAGE_TYPE_WITH_INFO.at(wx_usage::CONFIG.imageType).second.bitmapType);
                    } else {
                        bitmap.SaveFile(".\\clipboard-history\\image\\bitmap-"s + getTimeString() + wx_usage::FORMATTED_IMAGE_TYPE_WITH_INFO.at(wx_usage::BMP).second.extention, wx_usage::FORMATTED_IMAGE_TYPE_WITH_INFO.at(wx_usage::BMP).second.bitmapType);
                    } 
                } else {
                    std::cerr << "Bitmap Is Not OK" << "\r\n";      //error
                    wxLogError("Bitmap Is Not OK");     //error
                }
                logTextCtrl.AppendText("wxDF_DIB/wxDF_BITMAP "s + std::to_string(size) + "\r\n");
            } else {
                std::cout << "\r\n";
            }
        }
        if (wx_usage::ConfigStruct::ENABLE_TEXT & wx_usage::CONFIG.binaryData && wxTheClipboard->IsSupported(wxDF_TEXT)) {
            std::cout << "wxDF_TEXT" << " ";
            wxTextDataObject textDataObj{};
            if (wxTheClipboard->GetData(textDataObj)) {
                auto text{textDataObj.GetText()};
                size_t size = strlen(text.c_str());
                std::cout << size << "\r\n";
                bufferToFile(text.c_str(), size, ("clipboard-history/text/text-"s + getTimeString() + ".txt"s).c_str());
                logTextCtrl.AppendText("wxDF_TEXT "s + std::to_string(size) + "\r\n");
            } else {
                std::cout << "\r\n";
            }
        }
        if (wx_usage::ConfigStruct::ENABLE_HTML & wx_usage::CONFIG.binaryData && wxTheClipboard->IsSupported(wxDF_HTML)) {
            std::cout << "wxDF_HTML" << " ";
            wxHTMLDataObject htmlDataObj{};
            if (wxTheClipboard->GetData(htmlDataObj)) {
                size_t size = htmlDataObj.GetDataSize();
                std::cout << size << "\r\n";
                char *buffer = new char[size]{};
                htmlDataObj.GetDataHere(buffer);
                bufferToFile(buffer, size, ("clipboard-history/html/html-"s + getTimeString() + ".html"s).c_str());
                delete[] buffer;
                logTextCtrl.AppendText("wxDF_HTML "s + std::to_string(size) + "\r\n");
            } else {
                std::cout << "\r\n";
            }
        }
        std::cout << "= = = = = =" << "\r\n";
        logTextCtrl.AppendText("= = = = = = = = = =\r\n");
        wxTheClipboard->Close();
    }
    return text;

}

void CustomFrame::OnClose (wxCloseEvent& e) {
    wx_usage::writeConfig(wx_usage::CONFIG);
    general_usage::debug(wxString{} << "[Frame] " << "exit!");      //debug
    Destroy();
}
wxBEGIN_EVENT_TABLE(CustomFrame, wxFrame)
    EVT_CLOSE(CustomFrame::OnClose)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(CustomApp)

CustomApp::CustomApp(): customFrame{new CustomFrame{}} {}

bool CustomApp::OnInit() {       

    wxLog::SetActiveTarget(new CustomLogTarget{});

    // SetProcessDPIAware();

    ::wxInitAllImageHandlers();

    auto lam = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT CALLBACK {
        switch (message) {
        case WM_DRAWCLIPBOARD:
            static bool isFirstTime{true};
            if (!isFirstTime || wx_usage::ConfigStruct::SAVE_WHEN_STATRT & wx_usage::CONFIG.binaryData) {
                wxPostEvent(wxGetApp().getFrame(), wxCommandEvent{DRAW_CLIPBOARD_EVENT});
            }
            isFirstTime = false;
            break;
        case WM_CHANGECBCHAIN:
            if ((HWND)wParam == (HWND)windows_usage::hNextViewer) {
                windows_usage::hNextViewer = (HANDLE)lParam;
            } else if (windows_usage::hNextViewer) {
                SendMessage((HWND)windows_usage::hNextViewer, message, wParam, lParam);
            }
            break;
        case WM_DESTROY:
            ChangeClipboardChain(windows_usage::hWnd, (HWND)windows_usage::hNextViewer);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    };
    windows_usage::wcex.lpfnWndProc = lam;
    windows_usage::wcex.hInstance = GetModuleHandle(NULL);
    windows_usage::wcex.lpszClassName = _T("ClipboardMonitor");
    RegisterClassEx(&windows_usage::wcex);
    windows_usage::hWnd = CreateWindow(windows_usage::wcex.lpszClassName, TEXT(""), 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, windows_usage::wcex.hInstance, NULL);
    SetClipboardViewer(windows_usage::hWnd);

    customFrame->Show();
    return true;
}

int CustomApp::OnExit() {
    DestroyWindow(windows_usage::hWnd);
    general_usage::debug(wxString{} << "[App] " << "exit!");      //debug
    return 0;
}

wxFrame *CustomApp::getFrame() {
    return customFrame;
}

int main() {
    general_usage::debug(wxString{} << "[Only Terminal] " << "[main]");       //debug
    return wxEntry();
}