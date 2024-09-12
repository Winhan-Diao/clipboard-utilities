#include "file_operating.h"

void bufferToFile(const void *buffer, const size_t totalSize, const char *fileName) {
    wxFileName::Mkdir(wxFileName{fileName}.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if (totalSize <= 0) {
        general_usage::debug("Ignored writing to file because the buffer is empty.");       //debug
        return;
    }
    std::FILE *file = std::fopen(fileName, "wb");
    if (file) {
        std::fwrite(buffer, 1, totalSize, file);
        std::fclose(file);
    } else {
        std::cerr << "Failed to write to file" << "\r\n";
        wxLogError("Failed to write to file");
    }
    fclose(file);
}
