#ifndef UKIVE_TEXT_LANGUAGE_DETECTOR_H_
#define UKIVE_TEXT_LANGUAGE_DETECTOR_H_

#include <Windows.h>
#include <elscore.h>
#include <elssrvc.h>

#include "ukive/utils/string_utils.h"


namespace ukive {

    class LanguageDetector
    {
    private:
        PMAPPING_SERVICE_INFO mService;

    public:
        LanguageDetector();
        ~LanguageDetector();

        bool init();
        void close();

        bool detect(std::wstring text);
        bool detect(std::wstring text, std::uint32_t start);
    };

}

#endif  // UKIVE_TEXT_LANGUAGE_DETECTOR_H_