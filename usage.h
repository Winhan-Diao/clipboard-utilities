#ifndef USAGE_H
#define USAGE_H
#include "included_headers.h"

using namespace std::literals;
using namespace std::placeholders;

wxDECLARE_EVENT(DRAW_CLIPBOARD_EVENT, wxCommandEvent);

namespace windows_usage {
    extern HWND hWnd;
    extern HANDLE hNextViewer;
    extern WNDCLASSEX wcex;
}

namespace wx_usage {
    typedef unsigned char enumType;
    enum FormattedImageType: enumType {
        BMP,
        PNG,
        JPEG,
        XPM,
        ICO,
        _MAX,
    };
    struct FormattedImageInfo {
        wxBitmapType bitmapType;
        std::string extention;
    };
    const std::map<enumType, std::pair<std::string, FormattedImageInfo>> FORMATTED_IMAGE_TYPE_WITH_INFO{{BMP, {"BMP"s, {wxBITMAP_TYPE_BMP, ".bmp"s}}}
                                                                                                        , {PNG, {"PNG"s, {wxBITMAP_TYPE_PNG, ".png"s}}}
                                                                                                        , {JPEG, {"JPEG (non-alpha)"s, {wxBITMAP_TYPE_JPEG, ".jpeg"s}}}
                                                                                                        , {XPM, {"XPM"s, {wxBITMAP_TYPE_XPM, ".xpm"s}}}
                                                                                                        , {ICO, {"ICO (max scale 255)"s, {wxBITMAP_TYPE_ICO, ".ico"s}}}
                                                                                                        };
    
    enum CustomWindowID {
        ID_JMP_PAGE,
        ID_CUSTON_SPIN,
        ID_ICON_TO_FRAME,
    };
    extern wxCommandEvent event;
    constexpr struct ConfigStruct {
        using binaryDataType = unsigned short;
        using valueType = unsigned short;
        binaryDataType binaryData;
        valueType maxDay;
        valueType maxStorage;
        valueType maxCount;
        enumType imageType;
        
        static constexpr binaryDataType ENABLE_DIB = 1 << 0;
        static constexpr binaryDataType ENABLE_TEXT = 1 << 1;
        static constexpr binaryDataType ENABLE_UNICODE_TEXT = 1 << 2;
        static constexpr binaryDataType ENABLE_HTML = 1 << 3;

        static constexpr binaryDataType ENABLE_DIF = 1 << 4;
        static constexpr binaryDataType SAVE_WHEN_STATRT = 1 << 5;
        static constexpr binaryDataType CLEAN_BY_DAY = 1 << 6;
        static constexpr binaryDataType CLEAN_BY_STORAGE = 1 << 7;

        static constexpr binaryDataType CLEAN_BY_COUNT = 1 << 8;
        static constexpr binaryDataType STORE_FORMATTED_IMG = 1 << 9;
        static constexpr binaryDataType ENABLE_VERBOSE = 1 << 10;

    } DEFAULT_CONFIG{0b000'1001'1111, (ConfigStruct::valueType)-1, (ConfigStruct::valueType)-1, (ConfigStruct::valueType)-1, FormattedImageType::BMP};
    void writeConfig(const ConfigStruct& config);
    ConfigStruct loadConfig();
    extern std::remove_cv_t<decltype(DEFAULT_CONFIG)> CONFIG;
}

namespace general_usage {
    extern bool ENABLED;
    bool endianTest();
    extern bool isBE;
    #define debug(msg) _debug(msg, __FILE__, __LINE__, __FUNCTION__)
    extern std::mutex msgMutext;
    void _debug(const wxString& msg, const char* file, int line, const char* func);
}

#endif