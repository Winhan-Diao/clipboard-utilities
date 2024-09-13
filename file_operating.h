#pragma once
#include "usage.h"

void bufferToFile(const void *buffer, const size_t totalSize, const char *fileName);

bool isEmptyHTML(const std::string& text);

void scaleToValidICOSize(wxBitmap& bitmap);

// ==============================
//         Definition
// ==============================

