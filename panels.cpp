#include "panels.h"
#include "widgets.h"

IntroPanel::IntroPanel() = default;
IntroPanel::IntroPanel(wxWindow *parent): wxPanel{parent} {
    wxBoxSizer *sizer = new wxBoxSizer{wxVERTICAL}; 
    wxRichTextCtrl *richTextCtrl = new wxRichTextCtrl{this, wxID_ANY};
    richTextCtrl->SetEditable(false);
    
    wxFont font{10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Verdana"};
    richTextCtrl->SetFont(font);

    richTextCtrl->BeginFontSize(20);
    richTextCtrl->WriteText("Thanks for using Clipboard Utilities :-)\r\n");
    richTextCtrl->EndFontSize();
    richTextCtrl->Newline();

    richTextCtrl->BeginSymbolBullet(_T("•"), 10, 20);
    
    richTextCtrl->WriteText("This is an application that help you store your clipboard history.\r\n");
    richTextCtrl->WriteText("Currently support text (raw & UTF), HTML and picture (by default bitmap picture).\r\n");
    richTextCtrl->WriteText("The records are stored in \"clipboard-history\" directory under the same root.\r\n");
    richTextCtrl->WriteText("The recording details can be seen under \"log\" page.\r\n");
    richTextCtrl->EndNumberedBullet();


    sizer->Add(richTextCtrl, wxSizerFlags().Expand().Proportion(10));
    this->SetSizerAndFit(sizer);
    this->Hide();
}   

struct BaseCheckBoxFunc {
    void operator() (const wxCommandEvent& e) {
        updateConfigBinaryData();
        extraOperations();
        debugCode();        //debug
    };
    virtual void updateConfigBinaryData() = 0;
    virtual void extraOperations() {};
    virtual void debugCode() {};
    virtual ~BaseCheckBoxFunc() noexcept = default;
};

struct CheckBoxFunc : public BaseCheckBoxFunc {
    wxCheckBox *checkBox;
    wx_usage::ConfigStruct::binaryDataType shift;
    CheckBoxFunc() = default;
    CheckBoxFunc(wxCheckBox *checkBox, const wx_usage::ConfigStruct::binaryDataType& shift): checkBox(checkBox), shift(shift) {};
    void updateConfigBinaryData() override {
        if (checkBox->GetValue()) {
            wx_usage::CONFIG.binaryData |= shift; 
        } else {
            wx_usage::CONFIG.binaryData &= ~shift;
        }
    };
    void debugCode() override {
        general_usage::debug(wxString{"[CheckBoxFunc] [wx_usage::CONFIG.binaryData] "} << std::bitset<sizeof(decltype(shift)) * 8>(wx_usage::CONFIG.binaryData).to_string());        //debug
    }
};

struct AdditionalSpinCtrlCheckBoxFunc: public CheckBoxFunc {
private:
    wx_usage::ConfigStruct::valueType wx_usage::ConfigStruct::* configMemberPtr;
    int checkBoxIndex;
    wxSpinCtrl *spinCtrl;
public:
    AdditionalSpinCtrlCheckBoxFunc() = default;
    AdditionalSpinCtrlCheckBoxFunc(wxCheckBox *checkBox, const wx_usage::ConfigStruct::binaryDataType& shift, wx_usage::ConfigStruct::valueType wx_usage::ConfigStruct::* configMemberPtr)
        : CheckBoxFunc{checkBox, shift}
        , configMemberPtr(configMemberPtr) 
        , checkBoxIndex{checkBox->GetContainingSizer()->GetChildren().IndexOf(checkBox->GetContainingSizer()->GetItem(checkBox))}
        , spinCtrl{new wxSpinCtrl{checkBox->GetParent(), wx_usage::ID_CUSTON_SPIN, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, (wx_usage::ConfigStruct::valueType)-1, wx_usage::CONFIG.*configMemberPtr}}
    {
        checkBox->GetContainingSizer()->Insert(checkBoxIndex + 1, spinCtrl);
        spinCtrl->Bind(wxEVT_SPINCTRL, [this](auto& e){
            wx_usage::CONFIG.*(this->configMemberPtr) = spinCtrl->GetValue();
        });    
    };
    void extraOperations() override {
        spinCtrl->Show(checkBox->GetValue());
        checkBox->GetContainingSizer()->Layout();
        checkBox->GetParent()->GetContainingSizer()->Layout();
    }
    void debugCode() override {
        general_usage::debug(wxString{} << "[AdditionalSpinCtrlCheckBoxFunc] [wx_usage::CONFIG] " << std::bitset<sizeof(decltype(shift)) * 8>(wx_usage::CONFIG.binaryData).to_string() << ' ' << wx_usage::CONFIG.maxDay << ' ' << wx_usage::CONFIG.maxStorage << ' ' << wx_usage::CONFIG.maxCount);       //debug
    }
};

template <class T>
struct AdditionalComboBoxCheckBoxFunc;

template <class T>
struct AdditionalComboBoxCheckBoxFunc<std::map<wx_usage::enumType, std::pair<std::string, T>>>: public CheckBoxFunc {
private:
    wx_usage::enumType wx_usage::ConfigStruct::* configMemberPtr;
    const std::map<wx_usage::enumType, std::pair<std::string, wx_usage::FormattedImageInfo>>& enumMap;
    wxArrayString arrStr;
    int checkBoxIndex;
    wxComboBox *comboBox;
public:
    AdditionalComboBoxCheckBoxFunc() = default;
    template <class U>
    AdditionalComboBoxCheckBoxFunc(wxCheckBox *checkBox, const wx_usage::ConfigStruct::binaryDataType& shift, wx_usage::enumType wx_usage::ConfigStruct::* configMemberPtr, const std::map<wx_usage::enumType, std::pair<std::string, U>>& enumMap)
        : CheckBoxFunc{checkBox, shift}
        , configMemberPtr(configMemberPtr)
        , enumMap(enumMap)
        , arrStr{}
        , checkBoxIndex{checkBox->GetContainingSizer()->GetChildren().IndexOf(checkBox->GetContainingSizer()->GetItem(checkBox))}
        , comboBox{}
    {
        for (const auto& [type, info]: enumMap) {
            arrStr.Add(info.first);
        }
        std::stringstream ss{};
        ss << "[Only Terminal] " << "[vecStr] ";       //debug
        std::for_each(arrStr.begin(), arrStr.end(), [&ss](const auto& i){ss << i << ' ';});      //debug
        general_usage::debug(ss.str());     //debug

        comboBox = new wxComboBox(checkBox->GetParent(), wxID_ANY, enumMap.at(wx_usage::CONFIG.imageType).first, wxDefaultPosition, wxDefaultSize, arrStr);
        checkBox->GetContainingSizer()->Insert(checkBoxIndex + 1, comboBox);

        comboBox->Bind(wxEVT_COMBOBOX, [this](auto& e){
            std::cout << "[Selection] " << comboBox->GetSelection() << "\r\n";
            wx_usage::CONFIG.*(this->configMemberPtr) = comboBox->GetSelection();
        });
    };
    void extraOperations() override {
        comboBox->Show(checkBox->GetValue());
        checkBox->GetContainingSizer()->Layout();
        checkBox->GetParent()->GetContainingSizer()->Layout();
    }
    void debugCode() override {
        general_usage::debug(wxString{"[AdditionalComboBoxCheckBoxFunc] [wx_usage::CONFIG] "} << std::bitset<sizeof(decltype(shift)) * 8>(wx_usage::CONFIG.binaryData).to_string() << ' ' << wx_usage::CONFIG.maxDay << ' ' << wx_usage::CONFIG.maxStorage << ' ' << wx_usage::CONFIG.maxCount << ' ' << (unsigned short)wx_usage::CONFIG.imageType);     //debug
    }
};

template <class T>
std::function<std::shared_ptr<BaseCheckBoxFunc>(wxCheckBox *, const wx_usage::ConfigStruct::binaryDataType&)> comboBoxLamBuilder(wx_usage::enumType wx_usage::ConfigStruct::* configMemberPtr, const std::map<wx_usage::enumType, std::pair<std::string, T>>& m) {
    auto lam = [](wxCheckBox *checkBox, const wx_usage::ConfigStruct::binaryDataType& shift, wx_usage::enumType wx_usage::ConfigStruct::* l_configMemberPtr, const std::map<wx_usage::enumType, std::pair<std::string, T>>& l_m){
        return std::shared_ptr<AdditionalComboBoxCheckBoxFunc<std::map<wx_usage::enumType, std::pair<std::string, T>>>>(new AdditionalComboBoxCheckBoxFunc<std::map<wx_usage::enumType, std::pair<std::string, T>>>{checkBox, shift, l_configMemberPtr, l_m});
    };
    return std::bind(lam, _1, _2, configMemberPtr, m);
}


ConfigPanel::ConfigPanel() = default;
ConfigPanel::ConfigPanel(wxWindow *parent): wxPanel{parent} {
    wxSizerFlags checkBoxFlags = wxSizerFlags{}.Border(wxALL, 5);
    wxBoxSizer *generalSizer = new wxBoxSizer{wxVERTICAL};
    this->SetSizer(generalSizer);

    wxPanel *recordingConfigPanel = builder::buildVStaticBoxPanel(this, wxString{"Recording Config"});

    wxPanel *historyConfigPanel = builder::buildVStaticBoxPanel(this, wxString{"Storing Config"});

    wxPanel *loggingConfigPanel = builder::buildVStaticBoxPanel(this, wxString{"Logging Config"});

    std::function<std::shared_ptr<BaseCheckBoxFunc>(wxCheckBox *, const wx_usage::ConfigStruct::binaryDataType&)> checkBoxLam = [](wxCheckBox *checkBox, const wx_usage::ConfigStruct::binaryDataType& shift){
        return std::make_shared<CheckBoxFunc>(checkBox, shift);
    };
    std::function<std::shared_ptr<BaseCheckBoxFunc>(wxCheckBox *, const wx_usage::ConfigStruct::binaryDataType&, wx_usage::ConfigStruct::valueType wx_usage::ConfigStruct::*)> spinCtrlCheckBoxLam = [](wxCheckBox *checkBox, const wx_usage::ConfigStruct::binaryDataType& shift, wx_usage::ConfigStruct::valueType wx_usage::ConfigStruct::* configMemberPtr){
        return std::make_shared<AdditionalSpinCtrlCheckBoxFunc>(checkBox, shift, configMemberPtr);
    };
    std::array/* <std::tuple<wxPanel *, const char *, unsigned short, std::function<std::shared_ptr<BaseCheckBoxFunc> (wxCheckBox *, const wx_usage::ConfigStruct::type &)>>, 7> */ 
        checkBoxConfigInfo = {std::make_tuple(recordingConfigPanel, "Enable Text Recording", wx_usage::ConfigStruct::ENABLE_TEXT, checkBoxLam)
                            , std::make_tuple(recordingConfigPanel, "Enable Unicode Text Recording", wx_usage::ConfigStruct::ENABLE_UNICODE_TEXT, checkBoxLam)
                            , std::make_tuple(recordingConfigPanel, "Enable HTML Recording", wx_usage::ConfigStruct::ENABLE_HTML, checkBoxLam)
                            , std::make_tuple(recordingConfigPanel, "Enable Image Recording", wx_usage::ConfigStruct::ENABLE_DIB, checkBoxLam)
                            , std::make_tuple(recordingConfigPanel, "Enable DIF (sheets format) Recording", wx_usage::ConfigStruct::ENABLE_DIF, checkBoxLam)
                            , std::make_tuple(recordingConfigPanel, "Record once from clipboard when program starts.", wx_usage::ConfigStruct::SAVE_WHEN_STATRT, checkBoxLam)
                            , std::make_tuple(historyConfigPanel, "Clean data passed certain days. (days)", wx_usage::ConfigStruct::CLEAN_BY_DAY, (std::function<std::shared_ptr<BaseCheckBoxFunc>(wxCheckBox *, const wx_usage::ConfigStruct::binaryDataType&)>)std::bind(spinCtrlCheckBoxLam, _1, _2, &wx_usage::ConfigStruct::maxDay))
                            , std::make_tuple(historyConfigPanel, "Clean the oldest record when reaching certain storage limit. (MB)", wx_usage::ConfigStruct::CLEAN_BY_STORAGE, (std::function<std::shared_ptr<BaseCheckBoxFunc>(wxCheckBox *, const wx_usage::ConfigStruct::binaryDataType&)>)std::bind(spinCtrlCheckBoxLam, _1, _2, &wx_usage::ConfigStruct::maxStorage))
                            , std::make_tuple(historyConfigPanel, "Clean the oldest record by restricting the maximum count.", wx_usage::ConfigStruct::CLEAN_BY_COUNT, (std::function<std::shared_ptr<BaseCheckBoxFunc>(wxCheckBox *, const wx_usage::ConfigStruct::binaryDataType&)>)std::bind(spinCtrlCheckBoxLam, _1, _2, &wx_usage::ConfigStruct::maxCount))
                            , std::make_tuple(historyConfigPanel, "Save image as other formats (default BMP)", wx_usage::ConfigStruct::STORE_FORMATTED_IMG, comboBoxLamBuilder(&wx_usage::ConfigStruct::imageType, wx_usage::FORMATTED_IMAGE_TYPE_WITH_INFO))
                            , std::make_tuple(loggingConfigPanel, "Enable Verbose Logging", wx_usage::ConfigStruct::ENABLE_VERBOSE, checkBoxLam)
                            };
    for (auto& [panel, str, shift, checkBoxLam] : checkBoxConfigInfo) {
        auto checkBox = new wxCheckBox{panel, wxID_ANY, str};
        panel->GetSizer()->Add(checkBox, checkBoxFlags);
        checkBox->SetValue((wx_usage::CONFIG.binaryData & shift) != 0);        
        checkBox->Bind(wxEVT_CHECKBOX, [handler = checkBoxLam(checkBox, shift)](wxCommandEvent& e) {
            (*handler)(e);
        });
        wxPostEvent(checkBox, wxCommandEvent(wxEVT_CHECKBOX));
    }

    historyConfigPanel->GetSizer()->Add(new wxPanel{historyConfigPanel, wxID_ANY});


}   

