#include "test_window.h"

#include <functional>
#include <fstream>

#include <dwmapi.h>

#include "ukive/application.h"
#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/scroll_view.h"
#include "ukive/views/check_box.h"
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
#include "ukive/utils/xml/xml_parser.h"
#include "ukive/utils/xml/xml_writer.h"
#include "ukive/files/file.h"
#include "ukive/animation/interpolator.h"
#include "ukive/system/time_utils.h"

#include "shell/test/list/test_adapter.h"
#include "shell/resources/oigka_resources_id.h"


namespace shell {

    TestWindow::TestWindow()
        : Window(),
          dwm_button_(nullptr),
          image_view_(nullptr),
          check_box_(nullptr) {}

    TestWindow::~TestWindow() {
    }

    void TestWindow::onPreCreate(
        ukive::ClassInfo* info,
        int* win_style, int* win_ex_style) {
    }

    void TestWindow::onCreate() {
        Window::onCreate();

        animator_.setListener(this);
        animator_.setDuration(5 * 1000);
        animator_.setInitValue(0);
        animator_.setInterpolator(new ukive::LinearInterpolator(400));
        animator_.setRepeat(true);
        animator_.start();

        showTitleBar();
        inflateGroup();
        //inflateListView();
    }

    void TestWindow::onDraw(const ukive::Rect& rect) {
        animator_.update();

        Window::onDraw(rect);

        if (animator_.isRunning()) {
            invalidate();
        }
    }

    void TestWindow::onDestroy() {
        Window::onDestroy();

        animator_.stop();
    }

    void TestWindow::onClick(ukive::View* v) {
        if (v == dwm_button_) {
            if (isTitleBarShowing()) {
                hideTitleBar();
            } else {
                showTitleBar();
            }

            /*BOOL enable_aero = true;
            BOOL new_aero = true;
            ::DwmIsCompositionEnabled(&enable_aero);
            if (enable_aero) {
                ::DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
            } else {
                ::DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
            }

            ::DwmIsCompositionEnabled(&new_aero);*/
        }
    }

    void TestWindow::onAnimationProgress(ukive::Animator* animator) {
        image_view_->setX(animator->getCurValue());
        //invalidate();
    }

    void TestWindow::inflateGroup() {
        setContentView(Res::Layout::test_window_group_layout_xml);

        DXGI_OUTPUT_DESC outputDesc;
        DXGI_ADAPTER_DESC adapterDesc;
        auto adapter = ukive::Application::getGraphicDeviceManager()->getCurAdapter();
        auto output = ukive::Application::getGraphicDeviceManager()->getCurOutput();
        adapter->GetDesc(&adapterDesc);
        output->GetDesc(&outputDesc);

        std::wstring deviceDesc;
        deviceDesc.append(L"Device: ").append(adapterDesc.Description)
            .append(L"\n").append(L"Monitor: ").append(outputDesc.DeviceName);

        auto deviceTextView = static_cast<ukive::TextView*>(findViewById(Res::Id::tv_dev_text_params));
        deviceTextView->setText(deviceDesc);

        check_box_ = static_cast<ukive::CheckBox*>(findViewById(Res::Id::cb_anim_test));
        check_box_->setChecked(true);

        auto textView = static_cast<ukive::TextView*>(findViewById(Res::Id::tv_test_txt));
        textView->setText(L"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii这是一个示例程序，\n\n在这里可以显示文本。\n这是一个示例程序，\n在这里可以显示文本。\n这是一个示例程序，\n在这里可以显示文本。");
        textView->setBackground(new ukive::ColorDrawable(ukive::Color::White));

        ukive::UnderlineSpan* span = new ukive::UnderlineSpan(3, 5);
        textView->getEditable()->addSpan(span);

        image_view_ = static_cast<ukive::ImageView*>(findViewById(Res::Id::iv_test_img));
        std::wstring imgFileName = ukive::Application::getExecFileName(true);
        auto bitmap = ukive::BitmapFactory::decodeFile(this, ukive::File(imgFileName, L"freshpaint.png").getPath());
        image_view_->setImageBitmap(bitmap);
    }

    void TestWindow::inflateListView() {
        setContentView(Res::Layout::test_window_list_layout_xml);

        // Buttons
        dwm_button_ = static_cast<ukive::Button*>(findViewById(Res::Id::bt_dwm_button));
        dwm_button_->setOnClickListener(this);

        // ListView
        auto adapter = new TestAdapter();
        for (int i = 0; i < 36; ++i) {
            adapter->AddItem(0, L"test", L"test test");
        }

        auto list_view = static_cast<ukive::ListView*>(findViewById(Res::Id::lv_test_list));
        list_view->setLayouter(new ukive::GridListLayouter());
        list_view->setAdapter(adapter);
    }

}
