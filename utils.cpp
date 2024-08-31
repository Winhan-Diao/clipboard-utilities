#include "utils.h"

std::string getTimeString() {
    auto now = std::chrono::system_clock::now();
    std::stringstream ss{};
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    auto nano = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    ss << std::put_time(std::localtime(&timeT), "%Y-%m-%d-%H-%M-") << nano;
    return ss.str();
}

void bufferToFile(const void *buffer, const size_t totalSize, const char *fileName) {
    wxFileName::Mkdir(wxFileName{fileName}.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    if (totalSize <= 0) return;
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