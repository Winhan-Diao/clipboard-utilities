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
    std::cout << "OnDir " << dirname << "\r\n";        //debug
    static int t;       //debug
    while (checkIfToClean(dirname)) {
        std::cout << "traverse time: " << t++ << "\r\n";        //debug
        if (!removeOldest(dirname)) {
            std::cerr << "Somehow fail to clean file. Process interrupted." << "\r\n";
            break;
        }
    }
    return wxDIR_IGNORE;
}
wxDirTraverseResult RemoveOldestDirTraverser::OnFile(const wxString& filename) {
    return wxDIR_CONTINUE;
}


bool removeOldest(const wxString& directory) {
    // std::cout << "file to remove" << getOldest(directory) << "\r\n";        //debug
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
        std::cout << "config " << (wx_usage::CONFIG.maxStorage * 1024*1024) << "\r\n";        //debug
        std::cout << "now " << wxDir::GetTotalSize(directory) << "\r\n";        //debug
        if (wxDir::GetTotalSize(directory) > ((double)wx_usage::CONFIG.maxStorage * 1024*1024)) return true;
    }
    if (wx_usage::CONFIG.binaryData & wx_usage::ConfigStruct::CLEAN_BY_DAY) {
        wxString fileStr = getOldest(directory);
        // std::cout << "fileStr <" << fileStr << ">\r\n";     //debug
        if (!fileStr.IsEmpty() && wxFileName::Exists(fileStr)) {
            time_t t_t = wxFileName{fileStr}.GetModificationTime().GetTicks();
            std::cout << "file modified time: " << std::ctime(&t_t) << "\r\n";      //debug 
            if ((std::time(nullptr) - t_t) > (86400 * wx_usage::CONFIG.maxDay)) return true;
        }
    }
    if (wx_usage::CONFIG.binaryData & wx_usage::ConfigStruct::CLEAN_BY_COUNT) {
        CountFilesDirTraverser traverser{};
        wxDir dir{directory};
        dir.Traverse(traverser);
        std::cout << "traverser.getCount()" << traverser.getCount() << "\r\n";      //debug
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
