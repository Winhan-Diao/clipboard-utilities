#include "clipboard_operating.h"

bool UnicodeDataRecord::saveClipboardToDataObject() {
    return wxTheClipboard->GetData(*dataObject);
}
bool UnicodeDataRecord::saveDataObjectToBuffer() {
    size = reinterpret_cast<wxTextDataObject *>(dataObject)->GetDataSize() + 2;
    general_usage::debug(wxString{"[UnicodeDataRecord] "} << "cast size: " << reinterpret_cast<wxTextDataObject *>(dataObject)->GetDataSize());     //debug
    general_usage::debug(wxString{"[UnicodeDataRecord] "} << "non cast size: " << dataObject->GetDataSize(wxDF_UNICODETEXT));     //debug
    if (size != 2) {
        buffer = std::make_unique<std::string>(size, '\0');
        new(&(*buffer)[0]) int16_t{(int16_t)0xFEFF};
        dataObject->GetDataHere(wxDF_UNICODETEXT, &(*buffer)[2]);

        hash = std::hash<std::string>{}(*buffer);
        general_usage::debug(wxString{"[unicodeDataRecord] "} << "hash: " << hash);     //debug
        bool isDifferentFromLast = hash != wx_usage::LAST_HASH_DATA.unicodeLastHash;
        if (!isDifferentFromLast) general_usage::debug(wxString{"[unicodeDataRecord] "} << "hash the same from the last one");      //debug
        return isDifferentFromLast || !(wx_usage::ConfigStruct::AVOID_SAVING_THE_SAME & wx_usage::CONFIG.binaryData);
    } else {
        return false;
    }
}
void UnicodeDataRecord::saveBufferToFile() {
    bufferToFile(&(*buffer)[0], size, ("clipboard-history/unicode-text/unicode-text-"s + getTimeString() + ".txt"s).c_str());
}
void UnicodeDataRecord::saveHashFromBuffer() {
    wx_usage::LAST_HASH_DATA.unicodeLastHash = hash;
}


bool HTMLDataRecord::saveClipboardToDataObject() {
    return wxTheClipboard->GetData(*dataObject);
}
bool HTMLDataRecord::saveDataObjectToBuffer() {
    size = reinterpret_cast<UniversalDataObject *>(dataObject)->GetDataSize();
    general_usage::debug(wxString{"[HTMLDataRecord] "} << "cast size: " << reinterpret_cast<UniversalDataObject *>(dataObject)->GetDataSize());     //debug
    general_usage::debug(wxString{"[HTMLDataRecord] "} << "non cast size: " << dataObject->GetDataSize(wxDF_HTML));     //debug

    buffer = std::make_unique<std::string>(size, '\0');
    reinterpret_cast<UniversalDataObject *>(dataObject)->GetDataHere(&(*buffer)[0]);
    if (!isEmptyHTML(*buffer)) {
        hash = std::hash<std::string>{}(*buffer);
        general_usage::debug(wxString{"[HTMLDataRecord] "} << "hash: " << hash);     //debug
        bool isDifferentFromLast = hash != wx_usage::LAST_HASH_DATA.htmlLastHash;
        if (!isDifferentFromLast) general_usage::debug(wxString{"[HTMLDataRecord] "} << "hash the same from the last one");      //debug
        return isDifferentFromLast || !(wx_usage::ConfigStruct::AVOID_SAVING_THE_SAME & wx_usage::CONFIG.binaryData);
    } else {
        general_usage::debug("The html body is empty or of no body.");     //debug
        return false;
    }
}
void HTMLDataRecord::saveBufferToFile() {
    bufferToFile(&(*buffer)[0], size, ("clipboard-history/html/html-"s + getTimeString() + ".html"s).c_str());
}
void HTMLDataRecord::saveHashFromBuffer() {
    wx_usage::LAST_HASH_DATA.htmlLastHash = hash;
}


bool ImageDataRecord::saveClipboardToDataObject() {
    return wxTheClipboard->GetData(*dataObject);
}
bool ImageDataRecord::saveDataObjectToBuffer() {
    bitmap = std::make_unique<wxBitmap>(reinterpret_cast<wxBitmapDataObject *>(dataObject)->GetBitmap());
    if (bitmap->IsOk()) {
        wxImage img = bitmap->ConvertToImage();
        size_t size = img.GetHeight() * img.GetWidth() * 3;
        general_usage::debug(wxString{"[ImageDataRecord] "} << "size: " << size);     //debug
        std::string buffer = std::string(reinterpret_cast<char*>(img.GetData()), size);
        hash = std::hash<std::string>{}(buffer);
        general_usage::debug(wxString{"[ImageDataRecord] "} << "hash: " << hash);     //debug
        bool isDifferentFromLast = hash != wx_usage::LAST_HASH_DATA.imageLastHash;
        if (!isDifferentFromLast) general_usage::debug(wxString{"[ImageDataRecord] "} << "hash the same from the last one");      //debug
        return isDifferentFromLast || !(wx_usage::ConfigStruct::AVOID_SAVING_THE_SAME & wx_usage::CONFIG.binaryData);
    } else {
        general_usage::debug(wxString{"[ImageDataRecord] "} << "The bitmap is not OK");     //debug
        return false;
    }
}
void ImageDataRecord::saveBufferToFile() {
    bool isOfCustomFormat = wx_usage::CONFIG.binaryData & wx_usage::ConfigStruct::STORE_FORMATTED_IMG;
    wx_usage::enumType actualFormat = isOfCustomFormat? wx_usage::CONFIG.imageType: wx_usage::BMP;
    auto extension = wx_usage::FORMATTED_IMAGE_TYPE_WITH_INFO.at(actualFormat).second.extension;
    auto bitmapType = wx_usage::FORMATTED_IMAGE_TYPE_WITH_INFO.at(actualFormat).second.bitmapType;
    if (actualFormat == wx_usage::ICO)
        scaleToValidICOSize(*bitmap);
    wxFileName::Mkdir(".\\clipboard-history\\image\\", wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    bitmap->SaveFile(".\\clipboard-history\\image\\bitmap-"s + getTimeString() + extension, bitmapType);
}
void ImageDataRecord::saveHashFromBuffer() {
    wx_usage::LAST_HASH_DATA.imageLastHash = hash;
}

bool DIFDataRecord::saveClipboardToDataObject() {
    return wxTheClipboard->GetData(*dataObject);
}
bool DIFDataRecord::saveDataObjectToBuffer() {
    size = reinterpret_cast<UniversalDataObject *>(dataObject)->GetDataSize();
    general_usage::debug(wxString{"[DIFDataRecord] "} << "size: " << reinterpret_cast<UniversalDataObject *>(dataObject)->GetDataSize());     //debug
    if (size) {
        buffer = std::make_unique<std::string>(size, '\0');
        reinterpret_cast<UniversalDataObject *>(dataObject)->GetDataHere(&(*buffer)[0]);
        hash = std::hash<std::string>{}(*buffer);
        general_usage::debug(wxString{"[DIFDataRecord] "} << "hash: " << hash);     //debug
        bool isDifferentFromLast = hash != wx_usage::LAST_HASH_DATA.difLastHash;
        if (!isDifferentFromLast) general_usage::debug(wxString{"[DIFDataRecord] "} << "hash the same from the last one");      //debug
        return isDifferentFromLast || !(wx_usage::ConfigStruct::AVOID_SAVING_THE_SAME & wx_usage::CONFIG.binaryData);
    } else {
        return false;
    }
}
void DIFDataRecord::saveBufferToFile() {
    bufferToFile(&(*buffer)[0], size, ("clipboard-history/sheets/dif-"s + getTimeString() + ".dif"s).c_str());
}
void DIFDataRecord::saveHashFromBuffer() {
    wx_usage::LAST_HASH_DATA.difLastHash = hash;
}


bool TextDataRecord::saveClipboardToDataObject() {
    return wxTheClipboard->GetData(*dataObject);
}
bool TextDataRecord::saveDataObjectToBuffer() {
    buffer = std::make_unique<wxString>(reinterpret_cast<wxTextDataObject *>(dataObject)->GetText());
    general_usage::debug(wxString{"[TextDataRecord] "} << "size: " << reinterpret_cast<wxTextDataObject *>(dataObject)->GetTextLength());     //debug
    if (reinterpret_cast<wxTextDataObject *>(dataObject)->GetTextLength()) {
        hash = wxStringHash{}(*buffer);
        general_usage::debug(wxString{"[TextDataRecord] "} << "hash: " << hash);     //debug
        bool isDifferentFromLast = hash != wx_usage::LAST_HASH_DATA.textLastHash;
        if (!isDifferentFromLast) general_usage::debug(wxString{"[TextDataRecord] "} << "hash the same from the last one");      //debug
        return isDifferentFromLast || !(wx_usage::ConfigStruct::AVOID_SAVING_THE_SAME & wx_usage::CONFIG.binaryData);
    } else {
        return false;
    }
}
void TextDataRecord::saveBufferToFile() {
    wxFileName::Mkdir(".\\clipboard-history\\text\\", wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if (std::ofstream ofs{".\\clipboard-history\\text\\text-"s + getTimeString() + ".txt"s, std::ios::binary}) {
        ofs << *buffer;
    }
}
void TextDataRecord::saveHashFromBuffer() {
    wx_usage::LAST_HASH_DATA.textLastHash = hash;
}
