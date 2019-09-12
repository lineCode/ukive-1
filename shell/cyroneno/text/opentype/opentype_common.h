#ifndef CYRONENO_TEXT_OPENTYPE_OPENTYPE_COMMON_H_
#define CYRONENO_TEXT_OPENTYPE_OPENTYPE_COMMON_H_

#include "ukive/utils/convert.h"

#define READ_FONT_FILE(mem, size)  \
    font_file_.read(reinterpret_cast<char*>(&mem), size);  \
    if (!font_file_) { return false; }

#define READ_FONT_FILE_SWAP(mem, size)  \
    READ_FONT_FILE(mem, size)  \
    mem = ukive::swapBits(mem);

#define READ_FONT_FILE_TABLE(mem, size)  \
    file.read(reinterpret_cast<char*>(&mem), size);  \
    if (!file) { return false; }

#define READ_FONT_FILE_TABLE_SWAP(mem, size)  \
    READ_FONT_FILE_TABLE(mem, size)  \
    mem = ukive::swapBits(mem);

#endif  // CYRONENO_TEXT_OPENTYPE_OPENTYPE_COMMON_H_