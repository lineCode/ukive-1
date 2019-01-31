#include "ukive/utils/string_utils.h"

#include <algorithm>

#include <Windows.h>

#include "ukive/utils/unicode.h"


namespace ukive {

    string8 UTF16ToUTF8(const string16& str) {
        string8 utf8_str;
        if (Unicode::UTF16ToUTF8(str, &utf8_str)) {
            return utf8_str;
        }

        return {};
    }

    string16 UTF8ToUTF16(const string8& str) {
        string16 utf16_str;
        if (Unicode::UTF8ToUTF16(str, &utf16_str)) {
            return utf16_str;
        }

        return {};
    }

    string16 ANSIToUTF16(const string8& str) {
        int req_length = ::MultiByteToWideChar(
            CP_ACP, MB_PRECOMPOSED, str.data(), str.length(), nullptr, 0);
        if (req_length <= 0) {
            return {};
        }

        std::unique_ptr<WCHAR[]> w_buffer(new WCHAR[req_length]);

        req_length = ::MultiByteToWideChar(
            CP_ACP, MB_PRECOMPOSED,
            str.data(), str.length(), w_buffer.get(), req_length);
        if (req_length <= 0) {
            return {};
        }

        return string16(w_buffer.get(), req_length);
    }

    string16 toASCIILowerCase(const string16& str) {
        string16 result;
        result.reserve(str.size());
        for (auto ch : str) {
            if (ch >= 0x41 && ch <= 0x5A) {
                ch += 0x20;
            }
            result.push_back(ch);
        }
        return result;
    }

    string16 toASCIIUpperCase(const string16& str) {
        string16 result;
        result.reserve(str.size());
        for (auto ch : str) {
            if (ch >= 0x61 && ch <= 0x7A) {
                ch -= 0x20;
            }
            result.push_back(ch);
        }
        return result;
    }

    string8 trimString(const string8& str, bool all) {
        auto result = str;
        if (all) {
            for (size_t i = 0; i < result.length();) {
                if (result[i] == ' ') {
                    result.erase(i);
                } else {
                    ++i;
                }
            }
        } else {
            while (!result.empty()) {
                if (result[0] != ' ') {
                    break;
                }
                result.erase(0);
            }

            while (!result.empty()) {
                if (result[result.length() - 1] != ' ') {
                    break;
                }
                result.erase(result.length() - 1);
            }
        }

        return result;
    }

    string16 trimString(const string16& str, bool all) {
        auto result = str;
        if (all) {
            for (size_t i = 0; i < result.length();) {
                if (result[i] == L' ') {
                    result.erase(i);
                } else {
                    ++i;
                }
            }
        } else {
            while (!result.empty()) {
                if (result[0] != L' ') {
                    break;
                }
                result.erase(0);
            }

            while (!result.empty()) {
                if (result[result.length() - 1] != L' ') {
                    break;
                }
                result.erase(result.length() - 1);
            }
        }

        return result;
    }

    std::vector<string8> splitString(const string8& str, const string8& token) {
        size_t prev_index = 0;
        std::vector<string8> str_vec;
        for (;;) {
            auto cur_index = str.find(",", prev_index);
            if (cur_index != string8::npos) {
                auto indiv = str.substr(prev_index, cur_index - prev_index);
                str_vec.push_back(indiv);
                prev_index = cur_index + 1;
            } else {
                auto indiv = str.substr(prev_index, str.length() - prev_index);
                str_vec.push_back(indiv);
                break;
            }
        }

        return str_vec;
    }

    std::vector<string16> splitString(const string16& str, const string16& token) {
        size_t prev_index = 0;
        std::vector<string16> str_vec;
        for (;;) {
            auto cur_index = str.find(L",", prev_index);
            if (cur_index != string16::npos) {
                auto indiv = str.substr(prev_index, cur_index - prev_index);
                str_vec.push_back(indiv);
                prev_index = cur_index + 1;
            } else {
                auto indiv = str.substr(prev_index, str.length() - prev_index);
                str_vec.push_back(indiv);
                break;
            }
        }

        return str_vec;
    }

}
