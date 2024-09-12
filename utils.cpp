#include "utils.h"

std::string getTimeString() {
    auto now = std::chrono::system_clock::now();
    std::stringstream ss{};
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    auto nano = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    ss << std::put_time(std::localtime(&timeT), "%Y-%m-%d-%H-%M-") << nano;
    return ss.str();
}
