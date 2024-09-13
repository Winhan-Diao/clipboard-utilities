#include "usage.h"


extern "C" [[deprecated]] int __stdcall CustomWinMainCRTStartup() {
    AddDllDirectory(L".\\libs");
    extern int main();
    return main();
}

namespace windows_usage {
    [[deprecated]] struct Setup{
        Setup() {
            SetDllDirectory(_T(".\\libs"));    
        }
    };
    Setup s{};
    HWND hWnd{nullptr};
    HANDLE hNextViewer{nullptr};
    WNDCLASSEX wcex{sizeof(WNDCLASSEX)};
}

namespace wx_usage {
    wxCommandEvent event{DRAW_CLIPBOARD_EVENT};
    void writeConfig(const ConfigStruct& config) {
        std::ofstream ofs{"config.dat", std::ios::out | std::ios::binary | std::ios::trunc};
        if (ofs.is_open()) {
            ofs.write(reinterpret_cast<const char *>(&config), sizeof(ConfigStruct));
            ofs.close();
        } else {
            std::cerr << "[error] Can't write config to file \"config.dat\"" << "\r\n";     //error
            wxLogError("[error] Can't write config to file \"config.dat\"");        //error
        }
    }
    ConfigStruct loadConfig() {
        std::ifstream ifs{"config.dat", std::ios::in | std::ios::binary};
        if (ifs.is_open()) {
            general_usage::debug(wxString{} << "[Only Terminal] " << "There is config.dat file.");     //debug
            ConfigStruct config;
            ifs.read(reinterpret_cast<char *>(&config), sizeof(DEFAULT_CONFIG));
            general_usage::debug(wxString{} << "[Only Terminal] " << (config.ENABLE_DIB & config.binaryData) << ' ' << (config.ENABLE_TEXT & config.binaryData) << ' ' << (config.ENABLE_UNICODE_TEXT & config.binaryData) << ' ' << (config.ENABLE_HTML & config.binaryData) << ' ' << (config.SAVE_WHEN_STATRT & config.binaryData));        //debug
            char b[sizeof(config.binaryData) * 2 + 1]{0};      //debug
            std::sprintf(b, "%X", config.binaryData);      //debug
            general_usage::debug(wxString{} << "[Only Terminal] " << "[config data] " << b);        //debug
            ifs.close();
            return config;
        } else {
            general_usage::debug(wxString{} << "[Only Terminal] " << "There is NO config.dat file.");      //debug
            writeConfig(DEFAULT_CONFIG);
            return DEFAULT_CONFIG;
        }
    }
    std::remove_cv_t<decltype(DEFAULT_CONFIG)> CONFIG{loadConfig()};
}

namespace general_usage {
    bool ENABLED{true};
    bool endianTest() {
        char *cArr = new char[sizeof(int32_t)]{};
        new(cArr) int32_t{0x01020304};
        bool isBE = cArr[0] == 0x01;
        delete[] cArr;
        return isBE;
    }
    bool isBE = endianTest();
    std::mutex msgMutext{};
    void _debug(const wxString& msg, const char* file, int line, const char* func) {
        if (wx_usage::ConfigStruct::ENABLE_VERBOSE & wx_usage::CONFIG.binaryData) {
            std::lock_guard<std::mutex> lg{msgMutext};
            std::cout << msg << "\r\n";
            wxLog::OnLog(wxLOG_Debug, msg, wxLogRecordInfo{file, line, func, "component_"});
        } 
    }
}

