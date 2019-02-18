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

        showTitleBar();

        {
            std::fstream reader("D:\\test.xml");

            auto cpos = reader.tellg();
            reader.seekg(0, std::ios_base::end);
            auto charSize = reader.tellg();
            reader.seekg(cpos);

            std::unique_ptr<char[]> buf(new char[charSize]());
            reader.read(buf.get(), charSize);

            string8 str(buf.get(), charSize);

            ukive::XMLParser parser;
            std::shared_ptr<ukive::xml::Element> root;
            if (!parser.parse(str, &root)) {
                DCHECK(false);
            }

            string8 xml_str;
            ukive::XMLWriter writer;
            if (!writer.write(*root, &xml_str)) {
                DCHECK(false);
            }
        }

        inflateGroup();
        //inflateListView();
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
        dwm_button_ = new ukive::Button(this);
        dwm_button_->setOnClickListener(this);
        dwm_button_->setText(L"Switch DWM");
        auto dwm_btn_lp = Rlp::Builder()
            .start(layout->getId(), Rlp::START, dpToPx(12))
            .top(layout->getId(), Rlp::TOP, dpToPx(12)).build();
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
            .start(layout->getId(), Rlp::START, dpToPx(8))
            .top(dwm_button_->getId(), Rlp::BOTTOM, dpToPx(8))
            .end(layout->getId(), Rlp::END, dpToPx(8))
            .bottom(layout->getId(), Rlp::BOTTOM, dpToPx(8)).build();
        layout->addView(list_view, list_lp);
    }

}