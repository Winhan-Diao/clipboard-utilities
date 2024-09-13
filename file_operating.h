#pragma once
#include "usage.h"

inline bool isEmptyHTML(const std::string& text);

void bufferToFile(const void *buffer, const size_t totalSize, const char *fileName);

// ==============================
//         Definition
// ==============================

inline bool isEmptyHTML(const std::string& text) {
/*     // The performance of std::regex is bad enough that program crashs, so I have to use custom implementation until the issue is fixed.
    // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86164
    std::regex bodyBeginRegex(R"(<body[^>]*>)");
    std::regex bodyEndRegex(R"(<\/body>)");
    std::regex emptyBodyRegex(R"(<body[^>]*>(\s|<!--.*?-->)*?<\/body>)", std::regex_constants::optimize);
    bool isEmpty{!(std::regex_search(t, bodyBeginRegex) && std::regex_search(t, bodyEndRegex)) || std::regex_search(std::forward<T>(t), emptyBodyRegex)};
    return isEmpty;
 */    
    std::string startBodyNonClosedTemplate = R"(<body)"s;
    char startBodyClosedTemplate = '>';
    std::string endBodyTemplate = R"(</body>)"s;
    std::string commentBeginTemplate = R"(<!--)"s;
    std::string commentEndTemplate = R"(-->)"s;

    // State if there's </body>
    auto endBodyIter = std::search(text.cbegin(), text.cend(), endBodyTemplate.cbegin(), endBodyTemplate.cend());
    if (endBodyIter == text.cend()) {
        wxLogError("Not of HTML form!");        //error
        std::cerr << "Not of HTML form!" << "\r\n";     //error
        return true;
    }

    // Span <body
    auto startBodyNonClosedIter = std::search(text.cbegin(), text.cend(), startBodyNonClosedTemplate.cbegin(), startBodyNonClosedTemplate.cend());
    if (startBodyNonClosedIter == text.cend()) {
        wxLogError("Not of HTML form!");        //error
        std::cerr << "Not of HTML form!" << "\r\n";     //error
        return true;
    }

    // Span > (end of start body)
    auto startBodyClosedIter = std::find(startBodyNonClosedIter, text.cend(), startBodyClosedTemplate);
    if (startBodyClosedIter == text.cend()) {
        wxLogError("Not of HTML form!");        //error
        std::cerr << "Not of HTML form!" << "\r\n";     //error
        return true;
    }

    auto iter = startBodyClosedIter + 1;
    
    do {
        iter = std::find_if_not(iter, endBodyIter, [](const char& c){
            return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
        });
        if (std::equal(iter, std::min(iter + commentBeginTemplate.length(), endBodyIter), commentBeginTemplate.cbegin(), commentBeginTemplate.cend())) {
            iter = std::search(iter, endBodyIter, commentEndTemplate.cbegin(), commentEndTemplate.cend());
            if (iter == endBodyIter) {
                return false;
            } else {
                iter += commentEndTemplate.length();
            }
        } else {
            return iter == endBodyIter;
        }
    } while (iter < endBodyIter);

    return true;
}
