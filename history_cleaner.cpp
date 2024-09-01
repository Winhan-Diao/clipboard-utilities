#include "history_cleaner.h"


wxDirTraverseResult CountFilesDirTraverser::OnDir(const wxString& dirname) {
    ++count;
    return wxDIR_IGNORE;
}
wxDirTraverseResult CountFilesDirTraverser::OnFile(const wxString& filename) {
    ++count;
    return wxDIR_CONTINUE;
}
size_t CountFilesDirTraverser::getCount() {
    return count;
}


wxDirTraverseResult RemoveOldestDirTraverser::OnDir(const wxString& dirname) {
    general_usage::debug(wxString{"OnDir "} << dirname);      //debug
    int t = 0;       //debug
    while (checkIfToClean(dirname)) {
        general_usage::debug(wxString{"traverse time: "} << t++);     //debug
        if (!removeOldest(dirname)) {
            std::cerr << "Somehow fail to clean file. Process interrupted." << "\r\n";      //error
            wxLogError("Somehow fail to clean file. Process interrupted.");     //error
            break;
        }
    }
    return wxDIR_IGNORE;
}
wxDirTraverseResult RemoveOldestDirTraverser::OnFile(const wxString& filename) {
    return wxDIR_CONTINUE;
}


bool removeOldest(const wxString& directory) {
    general_usage::debug(wxString{"file to remove"} << getOldest(directory));     //debug
    return wxRemoveFile(getOldest(directory));
}

wxString getOldest(const wxString& directory) {


    wxDir dir{directory};
    wxArrayString fileStrs;
    wxDir::GetAllFiles(directory, &fileStrs);
    auto iter = std::min_element(fileStrs.begin(), fileStrs.end(), [](const wxString& fileStr1, const wxString& fileStr2){
        return wxFileModificationTime(fileStr1) < wxFileModificationTime(fileStr2);
    });
    return (iter == fileStrs.end())? (const decltype(*iter))(wxEmptyString): const_cast<const decltype(*iter)>(*iter);

}

/* end with "\\" */
bool checkIfToClean(const wxString& directory) {
    if (wx_usage::CONFIG.binaryData & wx_usage::ConfigStruct::CLEAN_BY_STORAGE) {
        general_usage::debug(wxString{"config restricted storage "} << (wx_usage::CONFIG.maxStorage * 1024*1024));       //debug
        general_usage::debug(wxString{"current directory storage "} << wxDir::GetTotalSize(directory).GetValue());     //debug
        if (wxDir::GetTotalSize(directory) > ((double)wx_usage::CONFIG.maxStorage * 1024*1024)) return true;
    }
    if (wx_usage::CONFIG.binaryData & wx_usage::ConfigStruct::CLEAN_BY_DAY) {
        wxString fileStr = getOldest(directory);
        general_usage::debug(wxString{"fileStr <"} << fileStr << '>');        //debug
        if (!fileStr.IsEmpty() && wxFileName::Exists(fileStr)) {
            time_t t_t = wxFileName{fileStr}.GetModificationTime().GetTicks();
            general_usage::debug(wxString{"file modified timestamp: "} << std::ctime(&t_t));      //debug
            if ((std::time(nullptr) - t_t) > (86400 * wx_usage::CONFIG.maxDay)) return true;
        }
    }
    if (wx_usage::CONFIG.binaryData & wx_usage::ConfigStruct::CLEAN_BY_COUNT) {
        CountFilesDirTraverser traverser{};
        wxDir dir{directory};
        dir.Traverse(traverser);
        general_usage::debug(wxString{"traverser.getCount() "} << traverser.getCount());      //debug
        if (traverser.getCount() > wx_usage::CONFIG.maxCount) return true;
    }
    return false;
}

    // if (fn.Exists())
    //     std::cout << "size " << fn.GetSize().GetLo() << "\r\n";
    // else 
    //     std::cerr << "No such file" << "\r\n";

    // if (!f.IsEmpty()) {
    //     if (!wxRemoveFile(f)) {
    //         std::cerr << "[getOldest] " << "Failed to delete a file." << "\r\n";
    //     } 
    // }
