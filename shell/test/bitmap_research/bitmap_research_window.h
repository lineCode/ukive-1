#ifndef SHELL_TEST_BITMAP_RESEARCH_BITMAP_RESEARCH_WINDOW_H_
#define SHELL_TEST_BITMAP_RESEARCH_BITMAP_RESEARCH_WINDOW_H_

#include "ukive/window/window.h"


namespace shell {

    class BitmapDumpView;

    class BitmapResearchWindow : public ukive::Window {
    public:
        void onCreate() override;
        bool onDataCopy(unsigned int id, unsigned int size, void *data) override;

    private:
        BitmapDumpView *bitmap_dump_view_;
    };

}

#endif  // SHELL_TEST_BITMAP_RESEARCH_BITMAP_RESEARCH_WINDOW_H_