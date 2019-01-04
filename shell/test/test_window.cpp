#include "test_window.h"

#include <functional>

#include <dwmapi.h>

#include "ukive/application.h"
#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/scroll_view.h"
#include "ukive/views/image_view.h"
#include "ukive/text/span/underline_span.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/graphics/color.h"
#include "ukive/views/list/list_adapter.h"
#include "ukive/views/list/list_view.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/graphics/color.h"
#include "ukive/utils/weak_bind.h"
#include "ukive/views/list/grid_list_layouter.h"
#include "ukive/views/list/linear_list_layouter.h"

#include "shell/test/list/test_adapter.h"


namespace {
    enum {
        kTextViewId = 1001,
        kImageViewId,
    };
}

namespace shell {

    TestWindow::TestWindow()
        : Window(),
          min_button_(nullptr),
          max_button_(nullptr),
          dwm_button_(nullptr) {
    }

    TestWindow::~TestWindow() {
    }


    void TestWindow::onPreCreate(
        ukive::ClassInfo* info,
        int* win_style, int* win_ex_style) {
    }

    void TestWindow::onCreate() {
        Window::onCreate();

        //inflateGroup();
        inflateListView();
    }

    void TestWindow::onClick(ukive::View* v) {
        if (v == min_button_) {
            minimize();
        } else if (v == max_button_) {
            if (isMaximum()) {
                restore();
            } else {
                maximize();
            }
        } else if (v == dwm_button_) {
            BOOL enable_aero = true;
            BOOL new_aero = true;
            ::DwmIsCompositionEnabled(&enable_aero);
            if (enable_aero) {
                ::DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
            } else {
                ::DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
            }

            ::DwmIsCompositionEnabled(&new_aero);
        }
    }

    void TestWindow::inflateGroup() {
        auto scrollView = new ukive::ScrollView(this);
        scrollView->setLayoutParams(
            new ukive::LayoutParams(ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::MATCH_PARENT));

        setContentView(scrollView);

        auto linearLayout = new ukive::LinearLayout(this);
        scrollView->addView(linearLayout,
            new ukive::LayoutParams(ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::MATCH_PARENT));

        DXGI_OUTPUT_DESC outputDesc;
        DXGI_ADAPTER_DESC adapterDesc;
        auto adapter = ukive::Application::getGraphicDeviceManager()->getCurAdapter();
        auto output = ukive::Application::getGraphicDeviceManager()->getCurOutput();
        adapter->GetDesc(&adapterDesc);
        output->GetDesc(&outputDesc);

        std::wstring adapterName(adapterDesc.Description);
        std::wstring outputName(outputDesc.DeviceName);

        std::wstring deviceDesc;
        deviceDesc.append(L"Device: ").append(adapterName)
            .append(L"\n").append(L"Monitor: ").append(outputName);


        ukive::TextView* deviceTextView = new ukive::TextView(this);
        deviceTextView->setIsSelectable(false);
        deviceTextView->setIsEditable(false);
        deviceTextView->setText(deviceDesc);
        deviceTextView->setPadding(dpToPx(6), dpToPx(6), dpToPx(6), dpToPx(6));

        ukive::LayoutParams* deviceTextParams = new ukive::LayoutParams(
            ukive::LayoutParams::FIT_CONTENT,
            ukive::LayoutParams::FIT_CONTENT);
        deviceTextParams->leftMargin = deviceTextParams->rightMargin
            = deviceTextParams->topMargin = deviceTextParams->bottomMargin = dpToPx(12);

        linearLayout->addView(deviceTextView, deviceTextParams);


        ukive::LayoutParams* textParams = new ukive::LayoutParams(
            ukive::LayoutParams::MATCH_PARENT,
            ukive::LayoutParams::FIT_CONTENT);
        textParams->leftMargin = textParams->rightMargin
            = textParams->topMargin = textParams->bottomMargin = dpToPx(12);

        ukive::TextView* textView = new ukive::TextView(this);
        textView->setId(kTextViewId);
        textView->setIsSelectable(true);
        textView->setIsEditable(true);
        textView->setText(L"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii这是一个示例程序，\n\n在这里可以显示文本。\n这是一个示例程序，\n在这里可以显示文本。\n这是一个示例程序，\n在这里可以显示文本。");
        textView->setBackground(new ukive::ColorDrawable(ukive::Color::White));
        textView->setPadding(dpToPx(6), dpToPx(6), dpToPx(6), dpToPx(6));
        textView->setElevation(3.f);

        ukive::UnderlineSpan* span = new ukive::UnderlineSpan(3, 5);
        textView->getEditable()->addSpan(span);

        linearLayout->addView(textView, textParams);

        std::wstring imgFileName = ukive::Application::getExecFileName(true);
        auto bitmap = ukive::BitmapFactory::decodeFile(this, imgFileName + L"\\freshpaint.png");
        ukive::ImageView* imageView = new ukive::ImageView(this);
        imageView->setId(kImageViewId);
        imageView->setImageBitmap(bitmap);

        linearLayout->addView(imageView);


        ukive::Button* button = new ukive::Button(this);
        ukive::LayoutParams* buttonParams = new ukive::LayoutParams(
            ukive::LayoutParams::FIT_CONTENT,
            ukive::LayoutParams::FIT_CONTENT);
        buttonParams->leftMargin = buttonParams->rightMargin
            = buttonParams->topMargin = buttonParams->bottomMargin = dpToPx(12);

        linearLayout->addView(button, buttonParams);
    }

    void TestWindow::inflateListView() {
        using Rlp = ukive::RestraintLayoutParams;
        auto layout = new ukive::RestraintLayout(this);
        layout->setLayoutParams(
            new ukive::LayoutParams(
                ukive::LayoutParams::MATCH_PARENT,
                ukive::LayoutParams::MATCH_PARENT));
        setContentView(layout);

        // Buttons
        min_button_ = new ukive::Button(this);
        min_button_->setOnClickListener(this);
        min_button_->setText(L"Minimum");
        auto min_btn_lp = Rlp::Builder()
            .start(layout->getId(), Rlp::START, dpToPx(4))
            .top(layout->getId(), Rlp::TOP, dpToPx(4)).build();
        layout->addView(min_button_, min_btn_lp);

        max_button_ = new ukive::Button(this);
        max_button_->setOnClickListener(this);
        max_button_->setText(L"Maximum");
        auto max_btn_lp = Rlp::Builder()
            .start(min_button_->getId(), Rlp::END, dpToPx(4))
            .top(layout->getId(), Rlp::TOP, dpToPx(4)).build();
        layout->addView(max_button_, max_btn_lp);

        dwm_button_ = new ukive::Button(this);
        dwm_button_->setOnClickListener(this);
        dwm_button_->setText(L"Switch DWM");
        auto dwm_btn_lp = Rlp::Builder()
            .start(max_button_->getId(), Rlp::END, dpToPx(4))
            .top(layout->getId(), Rlp::TOP, dpToPx(4)).build();
        layout->addView(dwm_button_, dwm_btn_lp);

        // ListView
        auto adapter = new TestAdapter();
        for (int i = 0; i < 36; ++i) {
            adapter->AddItem(0, L"test", L"test test");
        }

        auto list_view = new ukive::ListView(this);
        list_view->setLayouter(new ukive::GridListLayouter());
        list_view->setAdapter(adapter);

        auto list_lp = Rlp::Builder(
            ukive::LayoutParams::MATCH_PARENT,
            ukive::LayoutParams::MATCH_PARENT)
            .start(layout->getId(), Rlp::START, dpToPx(2))
            .top(min_button_->getId(), Rlp::BOTTOM, dpToPx(4))
            .end(layout->getId(), Rlp::END, dpToPx(2))
            .bottom(layout->getId(), Rlp::BOTTOM, dpToPx(2)).build();
        layout->addView(list_view, list_lp);
    }

}