#ifndef SHELL_LOD_LOD_WINDOW_H_
#define SHELL_LOD_LOD_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/views/click_listener.h"
#include "ukive/views/seek_value_changed_listener.h"


namespace ukive {
    class TextView;
    class SeekBar;
    class RestraintLayout;
    class Direct3DView;
}

namespace shell {

    class TerrainScene;

    class LodWindow :
        public ukive::Window,
        public ukive::OnSeekValueChangedListener,
        public ukive::OnClickListener
    {
    public:
        LodWindow();
        ~LodWindow();

        void onCreate() override;

        void onClick(ukive::View* widget) override;

        void onSeekValueChanged(ukive::SeekBar* seekBar, float value) override;
        void onSeekIntegerValueChanged(ukive::SeekBar* seekBar, int value) override;

        void onRender();

    private:
        void inflateCtlLayout(ukive::RestraintLayout* rightLayout);

        int mFrameCounter = 0;
        int mFramePreSecond = 0;
        uint64_t mPrevTime = 0;

        ukive::SeekBar* c1_seekbar_ = nullptr;
        ukive::SeekBar* c2_seekbar_ = nullptr;
        ukive::SeekBar* split_seekbar_ = nullptr;
        ukive::TextView* c1_value_tv_ = nullptr;
        ukive::TextView* c2_value_tv_ = nullptr;
        ukive::TextView* split_value_tv_ = nullptr;
        ukive::TextView* render_info_ = nullptr;
        ukive::Direct3DView* lod_view_ = nullptr;

        TerrainScene* terrain_scene_ = nullptr;
    };

}

#endif  // SHELL_LOD_LOD_WINDOW_H_