#include "test_window.h"

#include "ukive/application.h"
#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/views/scroll_view.h"
#include "ukive/views/image_view.h"
#include "ukive/text/span/underline_span.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/graphics/bitmap_factory.h"
#include "ukive/graphics/color.h"


namespace {
    const int kTextViewId = 1001;
    const int kImageViewId = 1002;
}

namespace shell {

    TestWindow::TestWindow()
        :Window() {
    }

    TestWindow::~TestWindow() {
    }


    void TestWindow::onCreate() {
        Window::onCreate();

        ukive::ScrollView *scrollView = new ukive::ScrollView(this);
        scrollView->setLayoutParams(
            new ukive::LayoutParams(ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::MATCH_PARENT));

        setContentView(scrollView);

        ukive::LinearLayout *linearLayout = new ukive::LinearLayout(this);
        scrollView->addWidget(linearLayout,
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


        ukive::TextView *deviceTextView = new ukive::TextView(this);
        deviceTextView->setIsSelectable(false);
        deviceTextView->setIsEditable(false);
        deviceTextView->setText(deviceDesc);
        deviceTextView->setPadding(dpToPx(6), dpToPx(6), dpToPx(6), dpToPx(6));

        ukive::LayoutParams *deviceTextParams = new ukive::LayoutParams(
            ukive::LayoutParams::FIT_CONTENT,
            ukive::LayoutParams::FIT_CONTENT);
        deviceTextParams->leftMargin = deviceTextParams->rightMargin
            = deviceTextParams->topMargin = deviceTextParams->bottomMargin = dpToPx(12);

        linearLayout->addWidget(deviceTextView, deviceTextParams);


        ukive::LayoutParams *textParams = new ukive::LayoutParams(
            ukive::LayoutParams::MATCH_PARENT,
            ukive::LayoutParams::FIT_CONTENT);
        textParams->leftMargin = textParams->rightMargin
            = textParams->topMargin = textParams->bottomMargin = dpToPx(12);

        ukive::TextView *textView = new ukive::TextView(this, kTextViewId);
        textView->setIsSelectable(true);
        textView->setIsEditable(true);
        textView->setText(L"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii这是一个示例程序，\n\n在这里可以显示文本。\n这是一个示例程序，\n在这里可以显示文本。\n这是一个示例程序，\n在这里可以显示文本。");
        textView->setBackground(new ukive::ColorDrawable(ukive::Color::White));
        textView->setPadding(dpToPx(6), dpToPx(6), dpToPx(6), dpToPx(6));
        textView->setElevation(3.f);

        ukive::UnderlineSpan *span = new ukive::UnderlineSpan(3, 5);
        textView->getEditable()->addSpan(span);

        linearLayout->addWidget(textView, textParams);

        std::wstring imgFileName(::_wgetcwd(nullptr, 0));
        auto bitmap = ukive::BitmapFactory::decodeFile(this, imgFileName + L"\\freshpaint.png");
        ukive::ImageView *imageView = new ukive::ImageView(this, kImageViewId);
        imageView->setImageBitmap(bitmap);

        linearLayout->addWidget(imageView);


        ukive::Button *button = new ukive::Button(this);
        ukive::LayoutParams *buttonParams = new ukive::LayoutParams(
            ukive::LayoutParams::FIT_CONTENT,
            ukive::LayoutParams::FIT_CONTENT);
        buttonParams->leftMargin = buttonParams->rightMargin
            = buttonParams->topMargin = buttonParams->bottomMargin = dpToPx(12);

        linearLayout->addWidget(button, buttonParams);
    }

}