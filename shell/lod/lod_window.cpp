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


namespace shell {

    LodWindow::LodWindow()
        :Window()
    {
    }

    LodWindow::~LodWindow()
    {
    }


    void LodWindow::onCreate()
    {
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
            = d3dViewLp->rightMargin = d3dViewLp->bottomMargin = 8;
        d3dViewLp->weight = 2;
        lodView->setLayoutParams(d3dViewLp);
        lodView->setBackground(new ukive::ColorDrawable(ukive::Color::White));
        lodView->setElevation(2);

        mLodView = lodView;

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


    void LodWindow::inflateCtlLayout(ukive::RestraintLayout *rightLayout)
    {
        /////////////////////////////第一行//////////////////////////////////
        //c1 label.
        ukive::TextView *c1Label = new ukive::TextView(this);
        c1Label->setId(ID_C1_LABEL);
        c1Label->setIsEditable(false);
        c1Label->setIsSelectable(false);
        c1Label->setText(L"C1值:");
        c1Label->setTextSize(13.f);

        ukive::RestraintLayoutParams *c1LabelLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::FIT_CONTENT, ukive::LayoutParams::FIT_CONTENT);
        c1LabelLp
            ->startHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::START, 8)
            ->topHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::TOP, 12);

        rightLayout->addView(c1Label, c1LabelLp);

        //c1 seekbar.
        ukive::SeekBar *c1SeekBar = new ukive::SeekBar(this);
        c1SeekBar->setId(ID_C1_SEEKBAR);
        c1SeekBar->setMaximum(60.f);
        c1SeekBar->setProgress(2.f - 1.f);
        c1SeekBar->setOnSeekValueChangedListener(this);

        ukive::RestraintLayoutParams *c1SeekBarLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::FIT_CONTENT);
        c1SeekBarLp
            ->startHandle(ID_C1_LABEL, ukive::RestraintLayoutParams::END, 4)
            ->topHandle(ID_C1_LABEL, ukive::RestraintLayoutParams::TOP)
            ->endHandle(ID_C1_VALUE, ukive::RestraintLayoutParams::START, 4)
            ->bottomHandle(ID_C1_LABEL, ukive::RestraintLayoutParams::BOTTOM);

        mC1SeekBar = c1SeekBar;
        rightLayout->addView(c1SeekBar, c1SeekBarLp);

        //c1 value.
        ukive::TextView *c1Value = new ukive::TextView(this);
        c1Value->setId(ID_C1_VALUE);
        c1Value->setIsEditable(false);
        c1Value->setIsSelectable(false);
        c1Value->setText(L"2.00");
        c1Value->setTextSize(13.f);

        ukive::RestraintLayoutParams *c1ValueLp = new ukive::RestraintLayoutParams(
            36, ukive::LayoutParams::FIT_CONTENT);
        c1ValueLp
            ->topHandle(ID_C1_LABEL, ukive::RestraintLayoutParams::TOP)
            ->endHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::END, 8)
            ->bottomHandle(ID_C1_LABEL, ukive::RestraintLayoutParams::BOTTOM);

        mC1ValueTV = c1Value;
        rightLayout->addView(c1Value, c1ValueLp);

        /////////////////////////////第二行//////////////////////////////////
        //c2 label.
        ukive::TextView *c2Label = new ukive::TextView(this);
        c2Label->setId(ID_C2_LABEL);
        c2Label->setIsEditable(false);
        c2Label->setIsSelectable(false);
        c2Label->setText(L"C2值:");
        c2Label->setTextSize(13.f);

        ukive::RestraintLayoutParams *c2LabelLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::FIT_CONTENT, ukive::LayoutParams::FIT_CONTENT);
        c2LabelLp
            ->startHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::START, 8)
            ->topHandle(ID_C1_LABEL, ukive::RestraintLayoutParams::BOTTOM, 8);

        rightLayout->addView(c2Label, c2LabelLp);

        //c2 seekbar.
        ukive::SeekBar *c2SeekBar = new ukive::SeekBar(this);
        c2SeekBar->setId(ID_C2_SEEKBAR);
        c2SeekBar->setMaximum(60.f);
        c2SeekBar->setProgress(30.f - 1.f);
        c2SeekBar->setOnSeekValueChangedListener(this);

        ukive::RestraintLayoutParams *c2SeekBarLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::FIT_CONTENT);
        c2SeekBarLp
            ->startHandle(ID_C2_LABEL, ukive::RestraintLayoutParams::END, 4)
            ->topHandle(ID_C2_LABEL, ukive::RestraintLayoutParams::TOP)
            ->endHandle(ID_C2_VALUE, ukive::RestraintLayoutParams::START, 4)
            ->bottomHandle(ID_C2_LABEL, ukive::RestraintLayoutParams::BOTTOM);

        mC2SeekBar = c2SeekBar;
        rightLayout->addView(c2SeekBar, c2SeekBarLp);

        //c2 value.
        ukive::TextView *c2Value = new ukive::TextView(this);
        c2Value->setId(ID_C2_VALUE);
        c2Value->setIsEditable(false);
        c2Value->setIsSelectable(false);
        c2Value->setText(L"30.00");
        c2Value->setTextSize(13.f);

        ukive::RestraintLayoutParams *c2ValueLp = new ukive::RestraintLayoutParams(
            36, ukive::LayoutParams::FIT_CONTENT);
        c2ValueLp
            ->topHandle(ID_C2_LABEL, ukive::RestraintLayoutParams::TOP)
            ->endHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::END, 8)
            ->bottomHandle(ID_C2_LABEL, ukive::RestraintLayoutParams::BOTTOM);

        mC2ValueTV = c2Value;
        rightLayout->addView(c2Value, c2ValueLp);

        /////////////////////////////第三行//////////////////////////////////
        //split label.
        ukive::TextView *splitLabel = new ukive::TextView(this);
        splitLabel->setId(ID_SPLIT_LABEL);
        splitLabel->setIsEditable(false);
        splitLabel->setIsSelectable(false);
        splitLabel->setText(L"分割:");
        splitLabel->setTextSize(13.f);

        ukive::RestraintLayoutParams *splitLabelLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::FIT_CONTENT, ukive::LayoutParams::FIT_CONTENT);
        splitLabelLp->startHandledId = ID_RIGHT_RESTRAIN;
        splitLabelLp->startHandledEdge = ukive::RestraintLayoutParams::START;
        splitLabelLp->leftMargin = 8;
        splitLabelLp->topHandledId = ID_C2_LABEL;
        splitLabelLp->topHandledEdge = ukive::RestraintLayoutParams::BOTTOM;
        splitLabelLp->topMargin = 16;

        rightLayout->addView(splitLabel, splitLabelLp);

        //split seekbar.
        ukive::SeekBar *splitSeekBar = new ukive::SeekBar(this);
        splitSeekBar->setId(ID_SPLIT_SEEKBAR);
        splitSeekBar->setMaximum(10.f);
        splitSeekBar->setProgress(5.f - 1.f);
        splitSeekBar->setOnSeekValueChangedListener(this);

        ukive::RestraintLayoutParams *splitSeekBarLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::FIT_CONTENT);
        splitSeekBarLp
            ->startHandle(ID_C2_SEEKBAR, ukive::RestraintLayoutParams::START)
            ->topHandle(ID_SPLIT_LABEL, ukive::RestraintLayoutParams::TOP)
            ->endHandle(ID_SPLIT_VALUE, ukive::RestraintLayoutParams::START, 4)
            ->bottomHandle(ID_SPLIT_LABEL, ukive::RestraintLayoutParams::BOTTOM);

        mSplitSeekBar = splitSeekBar;
        rightLayout->addView(splitSeekBar, splitSeekBarLp);

        //split value.
        ukive::TextView *splitValue = new ukive::TextView(this);
        splitValue->setId(ID_SPLIT_VALUE);
        splitValue->setIsEditable(false);
        splitValue->setIsSelectable(false);
        splitValue->setText(L"5");
        splitValue->setTextSize(13.f);

        ukive::RestraintLayoutParams *splitValueLp = new ukive::RestraintLayoutParams(
            36, ukive::LayoutParams::FIT_CONTENT);
        splitValueLp->topHandledId = ID_SPLIT_LABEL;
        splitValueLp->topHandledEdge = ukive::RestraintLayoutParams::TOP;
        splitValueLp->bottomHandledId = ID_SPLIT_LABEL;
        splitValueLp->bottomHandledEdge = ukive::RestraintLayoutParams::BOTTOM;
        splitValueLp->endHandledId = ID_RIGHT_RESTRAIN;
        splitValueLp->endHandledEdge = ukive::RestraintLayoutParams::END;
        splitValueLp->rightMargin = 8;

        mSplitValueTV = splitValue;
        rightLayout->addView(splitValue, splitValueLp);


        //submit button.
        ukive::Button *submitBT = new ukive::Button(this);
        submitBT->setId(ID_SUBMIT_BUTTON);
        submitBT->setText(L"提交");
        submitBT->setTextSize(12.f);
        submitBT->setOnClickListener(this);

        ukive::RestraintLayoutParams *submitBTLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::FIT_CONTENT, ukive::LayoutParams::FIT_CONTENT);
        submitBTLp->topMargin = 16;
        submitBTLp->topHandledId = ID_SPLIT_LABEL;
        submitBTLp->topHandledEdge = ukive::RestraintLayoutParams::BOTTOM;
        submitBTLp->endHandledId = ID_RIGHT_RESTRAIN;
        submitBTLp->endHandledEdge = ukive::RestraintLayoutParams::END;
        submitBTLp->rightMargin = 8;

        rightLayout->addView(submitBT, submitBTLp);

        //vsync button.
        ukive::Button *vsyncBT = new ukive::Button(this);
        vsyncBT->setId(ID_VSYNC_BUTTON);
        vsyncBT->setText(L"VSYNC ON");
        vsyncBT->setTextSize(12.f);
        vsyncBT->setTextColor(ukive::Color::White);
        vsyncBT->setTextWeight(DWRITE_FONT_WEIGHT_BOLD);
        vsyncBT->setButtonColor(ukive::Color::Blue500);
        vsyncBT->setOnClickListener(this);

        ukive::RestraintLayoutParams *vsyncBTLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::FIT_CONTENT, ukive::LayoutParams::FIT_CONTENT);
        vsyncBTLp->topMargin = 8;
        vsyncBTLp->topHandledId = ID_SUBMIT_BUTTON;
        vsyncBTLp->topHandledEdge = ukive::RestraintLayoutParams::BOTTOM;
        vsyncBTLp->endHandledId = ID_RIGHT_RESTRAIN;
        vsyncBTLp->endHandledEdge = ukive::RestraintLayoutParams::END;
        vsyncBTLp->rightMargin = 8;

        rightLayout->addView(vsyncBT, vsyncBTLp);


        //渲染参数显示器
        ukive::TextView *renderInfoTV = new ukive::TextView(this);
        renderInfoTV->setId(ID_LOD_INFO);
        renderInfoTV->setIsEditable(false);
        renderInfoTV->setIsSelectable(false);
        renderInfoTV->setText(L"Render info.");
        renderInfoTV->setTextSize(12.f);

        ukive::RestraintLayoutParams *renderInfoTVLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::FIT_CONTENT);
        renderInfoTVLp
            ->startHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::START, 8)
            ->topHandle(ID_VSYNC_BUTTON, ukive::RestraintLayoutParams::BOTTOM, 16)
            ->endHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::END, 8);

        rightLayout->addView(renderInfoTV, renderInfoTVLp);


        //帮助说明
        ukive::TextView *helperTV = new ukive::TextView(this);
        helperTV->setIsEditable(true);
        helperTV->setIsSelectable(true);
        helperTV->setText(
            L"►拖动滑条，点击提交按钮来更改参数。\
        \n►鼠标滚轮可进行缩放。\
        \n►按住Shift键和鼠标左键拖动可旋转摄像机。\
        \n►按住Ctrl键和鼠标左键拖动可移动摄像机");
        helperTV->setTextSize(14.f);
        helperTV->setBackground(new ukive::EditTextDrawable());
        helperTV->setPadding(4, 4, 4, 4);

        ukive::RestraintLayoutParams *helperTVLp = new ukive::RestraintLayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::FIT_CONTENT);
        helperTVLp
            ->startHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::START, 8)
            ->topHandle(ID_LOD_INFO, ukive::RestraintLayoutParams::BOTTOM, 24)
            ->endHandle(ID_RIGHT_RESTRAIN, ukive::RestraintLayoutParams::END, 8);

        rightLayout->addView(helperTV, helperTVLp);
    }


    void LodWindow::onClick(ukive::View *widget)
    {
        switch (widget->getId())
        {
        case ID_SUBMIT_BUTTON:
        {
            float c1 = mC1SeekBar->getProgress() + 1.f;
            float c2 = mC2SeekBar->getProgress() + 1.f;
            int splitCount = static_cast<int>(mSplitSeekBar->getProgress()) + 1;

            terrain_scene_->recreate(splitCount);
            terrain_scene_->reevaluate(c1, c2);
            mLodView->invalidate();

            break;
        }

        case ID_VSYNC_BUTTON:
        {
            ukive::Button *vsyncButton = static_cast<ukive::Button*>(widget);
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
            mC1ValueTV->setText(ukive::Float::toString(1.f + value, 2));
            break;

        case ID_C2_SEEKBAR:
            mC2ValueTV->setText(ukive::Float::toString(1.f + value, 2));
            break;
        }
    }

    void LodWindow::onSeekIntegerValueChanged(ukive::SeekBar *seekBar, int value)
    {
        switch (seekBar->getId())
        {
        case ID_SPLIT_SEEKBAR:
            mSplitValueTV->setText(std::to_wstring(1 + value));
            break;
        }
    }

}