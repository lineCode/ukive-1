#ifndef SHELL_LOD_LOD_WINDOW_H_
#define SHELL_LOD_LOD_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/seek_value_changed_listener.h"


namespace ukive {
    class TextView;
    class SeekBar;
    class RestraintLayout;
}

namespace shell {

    class LodView;

    class LodWindow :
        public ukive::Window,
        public ukive::OnSeekValueChangedListener,
        public ukive::OnClickListener
    {
    private:
        LodView *mLodView;
        ukive::TextView *mC1ValueTV;
        ukive::TextView *mC2ValueTV;
        ukive::TextView *mSplitValueTV;

        ukive::SeekBar *mC1SeekBar;
        ukive::SeekBar *mC2SeekBar;
        ukive::SeekBar *mSplitSeekBar;

        void inflateCtlLayout(ukive::RestraintLayout *rightLayout);

    public:
        static const int ID_LOD_INFO = 0x010;
        static const int ID_RIGHT_RESTRAIN = 0x011;

        static const int ID_C1_LABEL = 0x012;
        static const int ID_C1_SEEKBAR = 0x014;
        static const int ID_C1_VALUE = 0x015;

        static const int ID_C2_LABEL = 0x016;
        static const int ID_C2_SEEKBAR = 0x018;
        static const int ID_C2_VALUE = 0x019;

        static const int ID_SPLIT_LABEL = 0x01A;
        static const int ID_SPLIT_SEEKBAR = 0x01B;
        static const int ID_SPLIT_VALUE = 0x01C;

        static const int ID_SUBMIT_BUTTON = 0x01D;
        static const int ID_VSYNC_BUTTON = 0x01E;

        static const int ID_MONITOR = 0x01F;
        static const int ID_HELPER = 0x020;

    public:
        LodWindow();
        ~LodWindow();

        void onCreate() override;

        void onClick(ukive::View *widget);

        void onSeekValueChanged(ukive::SeekBar *seekBar, float value);
        void onSeekIntegerValueChanged(ukive::SeekBar *seekBar, int value);
    };

}

#endif  // SHELL_LOD_LOD_WINDOW_H_