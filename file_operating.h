#pragma once
#include "usage.h"

template <class T>
bool isEmptyHTML(T&& t);

void bufferToFile(const void *buffer, const size_t totalSize, const char *fileName);

// ==============================
//         Definition
// ==============================

template <class T>
bool isEmptyHTML(T&& t) {
    std::regex bodyBeginRegex(R"(<body[^>]*>)");
    std::regex bodyEndRegex(R"(<\/body>)");
    std::regex emptyBodyRegex(R"(<body[^>]*>(\s|<!--.*?-->)*?<\/body>)", std::regex_constants::optimize);
    bool isEmpty{!(std::regex_search(t, bodyBeginRegex) && std::regex_search(t, bodyEndRegex)) || std::regex_search(std::forward<T>(t), emptyBodyRegex)};
    return isEmpty;
}
