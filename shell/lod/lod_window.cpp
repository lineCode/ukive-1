#include "lod_window.h"

#include "ukive/application.h"
#include "ukive/graphics/color.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/views/seek_bar.h"
#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/drawable/edittext_drawable.h"
#include "ukive/utils/float.h"
#include "ukive/views/direct3d_view.h"

#include "shell/lod/terrain_scene.h"


namespace {
    enum {
        ID_LOD_INFO = 0x010,
        ID_RIGHT_RESTRAIN,

        ID_C1_LABEL,
        ID_C1_SEEKBAR,
        ID_C1_VALUE,

        ID_C2_LABEL,
        ID_C2_SEEKBAR,
        ID_C2_VALUE,

        ID_SPLIT_LABEL,
        ID_SPLIT_SEEKBAR,
        ID_SPLIT_VALUE,

        ID_SUBMIT_BUTTON,
        ID_VSYNC_BUTTON,

        ID_MONITOR,
        ID_HELPER,
    };
}

namespace shell {

    LodWindow::LodWindow()
        :Window() {
    }

    LodWindow::~LodWindow() {
    }


    void LodWindow::onPreCreate(
        ukive::ClassInfo* info,
        int* win_style, int* win_ex_style) {
    }

    void LodWindow::onCreate() {
        Window::onCreate();

        //root layout.
        ukive::LinearLayout *rootLayout = new ukive::LinearLayout(this);
        rootLayout->setOrientation(ukive::LinearLayout::HORIZONTAL);
        rootLayout->setLayoutParams(new ukive::LayoutParams(
            ukive::LayoutParams::MATCH_PARENT,
            ukive::LayoutParams::MATCH_PARENT));

        terrain_scene_ = new TerrainScene();

        //3d view.
        ukive::Direct3DView* lodView = new ukive::Direct3DView(this, terrain_scene_);
        ukive::LinearLayoutParams *d3dViewLp = new ukive::LinearLayoutParams(
            0, ukive::LayoutParams::MATCH_PARENT);
        d3dViewLp->leftMargin = d3dViewLp->topMargin
            = d3dViewLp->rightMargin = d3dViewLp->bottomMargin = dpToPx(8);
        d3dViewLp->weight = 2;
        lodView->setLayoutParams(d3dViewLp);
        lodView->setBackground(new ukive::ColorDrawable(ukive::Color::White));
        lodView->setElevation(dpToPx(2));

        lod_view_ = lodView;

        //right view.
        ukive::RestraintLayout *rightLayout = new ukive::RestraintLayout(this);
        rightLayout->setId(ID_RIGHT_RESTRAIN);
        ukive::LinearLayoutParams *rightViewLp = new ukive::LinearLayoutParams(
            0, ukive::LayoutParams::MATCH_PARENT);
        rightViewLp->weight = 1;
        rightLayout->setLayoutParams(rightViewLp);

        ////////////////////Right View Panel.

        inflateCtlLayout(rightLayout);

        ////////////////////End Panel////////////////////

        rootLayout->addView(lodView);
        rootLayout->addView(rightLayout);

        setContentView(rootLayout);
    }


    void LodWindow::inflateCtlLayout(ukive::RestraintLayout *rightLayout) {
        using Rlp = ukive::RestraintLayoutParams;

        /////////////////////////////第一行//////////////////////////////////
        //c1 label.
        ukive::TextView *c1Label = new ukive::TextView(this);
        c1Label->setId(ID_C1_LABEL);
        c1Label->setIsEditable(false);
        c1Label->setIsSelectable(false);
        c1Label->setText(L"C1值:");
        c1Label->setTextSize(13);

        auto c1LabelLp = Rlp::Builder(
            Rlp::FIT_CONTENT, Rlp::FIT_CONTENT)
            .start(ID_RIGHT_RESTRAIN, Rlp::START, dpToPx(8))
            .top(ID_RIGHT_RESTRAIN, Rlp::TOP, dpToPx(12)).build();

        rightLayout->addView(c1Label, c1LabelLp);

        //c1 seekbar.
        c1_seekbar_ = new ukive::SeekBar(this);
        c1_seekbar_->setId(ID_C1_SEEKBAR);
        c1_seekbar_->setMaximum(60.f);
        c1_seekbar_->setProgress(2.f - 1.f);
        c1_seekbar_->setOnSeekValueChangedListener(this);

        auto c1SeekBarLp = Rlp::Builder(
            Rlp::MATCH_PARENT, Rlp::FIT_CONTENT)
            .start(ID_C1_LABEL, Rlp::END, dpToPx(4))
            .top(ID_C1_LABEL)
            .end(ID_C1_VALUE, Rlp::START, dpToPx(4))
            .bottom(ID_C1_LABEL).build();

        rightLayout->addView(c1_seekbar_, c1SeekBarLp);

        //c1 value.
        c1_value_tv_ = new ukive::TextView(this);
        c1_value_tv_->setId(ID_C1_VALUE);
        c1_value_tv_->setIsEditable(false);
        c1_value_tv_->setIsSelectable(false);
        c1_value_tv_->setText(L"2.00");
        c1_value_tv_->setTextSize(13);

        auto c1ValueLp = Rlp::Builder(
            dpToPx(36), Rlp::FIT_CONTENT)
            .top(ID_C1_LABEL)
            .end(ID_RIGHT_RESTRAIN, Rlp::END, dpToPx(8))
            .bottom(ID_C1_LABEL).build();

        rightLayout->addView(c1_value_tv_, c1ValueLp);

        /////////////////////////////第二行//////////////////////////////////
        //c2 label.
        auto c2Label = new ukive::TextView(this);
        c2Label->setId(ID_C2_LABEL);
        c2Label->setIsEditable(false);
        c2Label->setIsSelectable(false);
        c2Label->setText(L"C2值:");
        c2Label->setTextSize(13);

        auto c2LabelLp = Rlp::Builder(
            Rlp::FIT_CONTENT, Rlp::FIT_CONTENT)
            .start(ID_RIGHT_RESTRAIN, Rlp::START, dpToPx(8))
            .top(ID_C1_LABEL, Rlp::BOTTOM, dpToPx(8)).build();

        rightLayout->addView(c2Label, c2LabelLp);

        //c2 seekbar.
        c2_seekbar_ = new ukive::SeekBar(this);
        c2_seekbar_->setId(ID_C2_SEEKBAR);
        c2_seekbar_->setMaximum(60.f);
        c2_seekbar_->setProgress(30.f - 1.f);
        c2_seekbar_->setOnSeekValueChangedListener(this);

        auto c2SeekBarLp = Rlp::Builder(
            Rlp::MATCH_PARENT, Rlp::FIT_CONTENT)
            .start(ID_C2_LABEL, Rlp::END, dpToPx(4))
            .top(ID_C2_LABEL)
            .end(ID_C2_VALUE, Rlp::START, dpToPx(4))
            .bottom(ID_C2_LABEL).build();

        rightLayout->addView(c2_seekbar_, c2SeekBarLp);

        //c2 value.
        c2_value_tv_ = new ukive::TextView(this);
        c2_value_tv_->setId(ID_C2_VALUE);
        c2_value_tv_->setIsEditable(false);
        c2_value_tv_->setIsSelectable(false);
        c2_value_tv_->setText(L"30.00");
        c2_value_tv_->setTextSize(13);

        auto c2ValueLp = Rlp::Builder(
            dpToPx(36), Rlp::FIT_CONTENT)
            .top(ID_C2_LABEL)
            .end(ID_RIGHT_RESTRAIN, Rlp::END, dpToPx(8))
            .bottom(ID_C2_LABEL).build();

        rightLayout->addView(c2_value_tv_, c2ValueLp);

        /////////////////////////////第三行//////////////////////////////////
        //split label.
        auto splitLabel = new ukive::TextView(this);
        splitLabel->setId(ID_SPLIT_LABEL);
        splitLabel->setIsEditable(false);
        splitLabel->setIsSelectable(false);
        splitLabel->setText(L"分割:");
        splitLabel->setTextSize(13);

        auto splitLabelLp = Rlp::Builder(
            Rlp::FIT_CONTENT, Rlp::FIT_CONTENT)
            .start(ID_RIGHT_RESTRAIN, Rlp::START, dpToPx(8))
            .top(ID_C2_LABEL, Rlp::BOTTOM, dpToPx(16)).build();

        rightLayout->addView(splitLabel, splitLabelLp);

        //split seekbar.
        split_seekbar_ = new ukive::SeekBar(this);
        split_seekbar_->setId(ID_SPLIT_SEEKBAR);
        split_seekbar_->setMaximum(10.f);
        split_seekbar_->setProgress(5.f - 1.f);
        split_seekbar_->setOnSeekValueChangedListener(this);

        auto splitSeekBarLp = Rlp::Builder(
            Rlp::MATCH_PARENT, Rlp::FIT_CONTENT)
            .start(ID_C2_SEEKBAR)
            .top(ID_SPLIT_LABEL)
            .end(ID_SPLIT_VALUE, Rlp::START, dpToPx(4))
            .bottom(ID_SPLIT_LABEL).build();

        rightLayout->addView(split_seekbar_, splitSeekBarLp);

        //split value.
        split_value_tv_ = new ukive::TextView(this);
        split_value_tv_->setId(ID_SPLIT_VALUE);
        split_value_tv_->setIsEditable(false);
        split_value_tv_->setIsSelectable(false);
        split_value_tv_->setText(L"5");
        split_value_tv_->setTextSize(13);

        auto splitValueLp = Rlp::Builder(
            dpToPx(36), Rlp::FIT_CONTENT)
            .end(ID_RIGHT_RESTRAIN, Rlp::END, dpToPx(8))
            .top(ID_SPLIT_LABEL)
            .bottom(ID_SPLIT_LABEL).build();

        rightLayout->addView(split_value_tv_, splitValueLp);


        //submit button.
        auto submitBT = new ukive::Button(this);
        submitBT->setId(ID_SUBMIT_BUTTON);
        submitBT->setText(L"提交");
        submitBT->setTextSize(12);
        submitBT->setOnClickListener(this);

        auto submitBTLp = Rlp::Builder(
            Rlp::FIT_CONTENT, Rlp::FIT_CONTENT)
            .end(ID_RIGHT_RESTRAIN, Rlp::END, dpToPx(8))
            .top(ID_SPLIT_LABEL, Rlp::BOTTOM, dpToPx(16)).build();

        rightLayout->addView(submitBT, submitBTLp);

        //vsync button.
        auto vsyncBT = new ukive::Button(this);
        vsyncBT->setId(ID_VSYNC_BUTTON);
        vsyncBT->setText(L"VSYNC ON");
        vsyncBT->setTextSize(12);
        vsyncBT->setTextColor(ukive::Color::White);
        vsyncBT->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        vsyncBT->setButtonColor(ukive::Color::Blue500);
        vsyncBT->setOnClickListener(this);

        auto vsyncBTLp = Rlp::Builder(
            Rlp::FIT_CONTENT, Rlp::FIT_CONTENT)
            .end(ID_RIGHT_RESTRAIN, Rlp::END, dpToPx(8))
            .top(ID_SUBMIT_BUTTON, Rlp::BOTTOM, dpToPx(8)).build();

        rightLayout->addView(vsyncBT, vsyncBTLp);


        //渲染参数显示器
        auto renderInfoTV = new ukive::TextView(this);
        renderInfoTV->setId(ID_LOD_INFO);
        renderInfoTV->setIsEditable(false);
        renderInfoTV->setIsSelectable(false);
        renderInfoTV->setText(L"Render info.");
        renderInfoTV->setTextSize(12);

        auto renderInfoTVLp = Rlp::Builder(
            Rlp::MATCH_PARENT, Rlp::FIT_CONTENT)
            .start(ID_RIGHT_RESTRAIN, Rlp::START, dpToPx(8))
            .top(ID_VSYNC_BUTTON, Rlp::BOTTOM, dpToPx(16))
            .end(ID_RIGHT_RESTRAIN, Rlp::END, dpToPx(8)).build();

        rightLayout->addView(renderInfoTV, renderInfoTVLp);


        //帮助说明
        auto helperTV = new ukive::TextView(this);
        helperTV->setId(ID_HELPER);
        helperTV->setIsEditable(true);
        helperTV->setIsSelectable(true);
        helperTV->setText(
            L"►拖动滑条，点击提交按钮来更改参数。\
        \n►鼠标滚轮可进行缩放。\
        \n►按住Shift键和鼠标左键拖动可旋转摄像机。\
        \n►按住Ctrl键和鼠标左键拖动可移动摄像机");
        helperTV->setTextSize(14);
        helperTV->setBackground(new ukive::EditTextDrawable(this));
        helperTV->setPadding(dpToPx(4), dpToPx(4), dpToPx(4), dpToPx(4));

        auto helperTVLp = Rlp::Builder(
            Rlp::MATCH_PARENT, Rlp::FIT_CONTENT)
            .start(ID_RIGHT_RESTRAIN, Rlp::START, dpToPx(8))
            .top(ID_LOD_INFO, Rlp::BOTTOM, dpToPx(24))
            .end(ID_RIGHT_RESTRAIN, Rlp::END, dpToPx(8)).build();

        rightLayout->addView(helperTV, helperTVLp);
    }


    void LodWindow::onClick(ukive::View *v)
    {
        switch (v->getId())
        {
        case ID_SUBMIT_BUTTON:
        {
            float c1 = c1_seekbar_->getProgress() + 1.f;
            float c2 = c2_seekbar_->getProgress() + 1.f;
            int splitCount = static_cast<int>(split_seekbar_->getProgress()) + 1;

            terrain_scene_->recreate(splitCount);
            terrain_scene_->reevaluate(c1, c2);
            lod_view_->invalidate();

            break;
        }

        case ID_VSYNC_BUTTON:
        {
            ukive::Button *vsyncButton = static_cast<ukive::Button*>(v);
            if (vsyncButton->getText() == L"VSYNC ON")
            {
                ukive::Application::setVSync(false);
                vsyncButton->setText(L"VSYNC OFF");
                vsyncButton->setButtonColor(ukive::Color::Yellow800);
            }
            else if (vsyncButton->getText() == L"VSYNC OFF")
            {
                ukive::Application::setVSync(true);
                vsyncButton->setText(L"VSYNC ON");
                vsyncButton->setButtonColor(ukive::Color::Blue500);
            }
            break;
        }
        }
    }

    void LodWindow::onSeekValueChanged(ukive::SeekBar *seekBar, float value)
    {
        switch (seekBar->getId())
        {
        case ID_C1_SEEKBAR:
            c1_value_tv_->setText(ukive::Float::toString(1.f + value, 2));
            break;

        case ID_C2_SEEKBAR:
            c2_value_tv_->setText(ukive::Float::toString(1.f + value, 2));
            break;
        }
    }

    void LodWindow::onSeekIntegerValueChanged(ukive::SeekBar *seekBar, int value)
    {
        switch (seekBar->getId())
        {
        case ID_SPLIT_SEEKBAR:
            split_value_tv_->setText(std::to_wstring(1 + value));
            break;
        }
    }

}