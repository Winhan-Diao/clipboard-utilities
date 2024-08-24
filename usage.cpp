#include "usage.h"


extern "C" [[deprecated]] int __stdcall CustomWinMainCRTStartup() {
    AddDllDirectory(L".\\libs");
    extern int main();
    return main();
}

namespace windows_usage {
    struct Setup{
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
        std::ofstream ofs("config.dat", std::ios::out | std::ios::binary);
        if (ofs) {
            ofs.write(reinterpret_cast<const char *>(&config), sizeof(DEFAULT_CONFIG));
        } else {
            std::cerr << "[error] Can't write config to file \"config.dat\"" << "\r\n";
        }
    }
    ConfigStruct loadConfig() {
        std::fstream fs{"config.dat", std::ios::in | std::ios::binary};
        if (fs) {
            std::cout << "There is config.dat file." << "\r\n";     //debug
            ConfigStruct config;
            fs.read(reinterpret_cast<char *>(&config), sizeof(DEFAULT_CONFIG));
            std::cout << (config.ENABLE_DIB & config.binaryData) << ' ' << (config.ENABLE_TEXT & config.binaryData) << ' ' << (config.ENABLE_UNICODE_TEXT & config.binaryData) << ' ' << (config.ENABLE_HTML & config.binaryData) << ' ' << (config.SAVE_WHEN_STATRT & config.binaryData) << "\r\n";        //debug
            std::cout << "[config data] "<< std::hex << config.binaryData << "\r\n";        //debug
            std::cout.unsetf(std::ios::hex);        //debug
            return config;
        } else {
            std::cout << "There is NO config.dat file." << "\r\n";      //debug
            writeConfig(DEFAULT_CONFIG);
            return DEFAULT_CONFIG;
        }
    }
    std::remove_cv_t<decltype(DEFAULT_CONFIG)> CONFIG{loadConfig()};
}

namespace general_usage {
    bool endianTest() {
        char *cArr = new char[sizeof(int32_t)]{};
        new(cArr) int32_t{0x01020304};
        bool isBE = cArr[0] == 0x01;
        delete[] cArr;
        return isBE;
    }
    bool isBE = endianTest();
}

