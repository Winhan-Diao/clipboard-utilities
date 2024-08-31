#pragma once
#include "included_headers.h"

std::string getTimeString();

void bufferToFile(const void *buffer, const size_t totalSize, const char *fileName);