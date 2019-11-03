#include "test_window.h"

#include <functional>
#include <fstream>

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
#include "ukive/views/spinner_view/spinner_view.h"
#include "ukive/utils/xml/xml_parser.h"
#include "ukive/utils/xml/xml_writer.h"
#include "ukive/files/file.h"
#include "ukive/animation/interpolator.h"
#include "ukive/system/time_utils.h"
#include "ukive/net/http_client.h"
#include "ukive/net/socket.h"
#include "ukive/net/tls.h"
#include "ukive/security/crypto/ecdp.h"

#include "shell/test/list/test_adapter.h"
#include "shell/resources/oigka_resources_id.h"
#include "shell/test/security/digest_unit_test.h"
#include "shell/test/utils/big_integer_unit_test.h"
#include "shell/test/utils/json_unit_test.h"
#include "shell/test/security/crypto_unit_test.h"
#include "shell/resources/resource.h"

#include "ukive/security/digest/md5.h"
#include "ukive/security/crypto/aes.h"
#include "ukive/security/crypto/rsa.h"
#include "ukive/security/crypto/aead.hpp"
#include "ukive/system/qpc_service.h"


namespace shell {

    TestWindow::TestWindow()
        : Window() {}

    TestWindow::~TestWindow() {
    }

    void TestWindow::onCreate() {
        Window::onCreate();

        setBackgroundColor(ukive::Color::Transparent);

        animator_.setListener(this);
        animator_.setDuration(5 * 1000);
        animator_.setInitValue(0);
        animator_.setInterpolator(new ukive::LinearInterpolator(400));
        animator_.setRepeat(true);
        //animator_.start();

        showTitleBar();
        inflateGroup();
        //inflateListView();

        //test::TEST_MD5();
        //test::TEST_SHA();
        //test::TEST_BIG_INTEGER();
        //test::TEST_JSON();
        //test::TEST_ECDP_X25519();
        //test::TEST_ECDP_X448();
        //test::TEST_AES();
        //test::TEST_AEAD_AES_GCM();

        //ukive::crypto::RSA rsa;
        //rsa.init();

        /*ukive::net::initializeSocket();

        ukive::net::tls::TLS tls_client;
        tls_client.testHandshake();

        ukive::net::unInitializeSocket();*/
    }

    void TestWindow::onDraw(const ukive::Rect& rect) {
        animator_.update();

        Window::onDraw(rect);
    }

    void TestWindow::onDestroy() {
        Window::onDestroy();

        animator_.stop();
    }

    bool TestWindow::onGetWindowIconName(string16* icon_name, string16* small_icon_name) const {
        *icon_name = IDI_SHELL;
        *small_icon_name = IDI_SMALL;
        return true;
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
        } else if (test_button_ == v) {
            int grt = 0;
        }
    }

    void TestWindow::onAnimationProgress(ukive::Animator* animator) {
        image_view_->setX(animator->getCurValue());
        image_view_->invalidate();
    }

    void TestWindow::inflateGroup() {
        setContentView(Res::Layout::test_window_group_layout_xml);

        getContentView()->setBackground(new ukive::ColorDrawable(ukive::Color::White));

        DXGI_OUTPUT_DESC outputDesc;
        DXGI_ADAPTER_DESC adapterDesc;
        auto adapter = ukive::Application::getGraphicDeviceManager()->getCurAdapter();
        auto output = ukive::Application::getGraphicDeviceManager()->getCurOutput();
        adapter->GetDesc(&adapterDesc);
        output->GetDesc(&outputDesc);

        std::wstring deviceDesc;
        deviceDesc.append(L"Device: ").append(adapterDesc.Description)
            .append(L"\n").append(L"Monitor: ").append(outputDesc.DeviceName);

        auto deviceTextView = findViewById<ukive::TextView>(Res::Id::tv_dev_text_params);
        deviceTextView->setText(deviceDesc);

        check_box_ = findViewById<ukive::CheckBox>(Res::Id::cb_anim_test);
        check_box_->setChecked(true);

        spinner_view_ = findViewById<ukive::SpinnerView>(Res::Id::sv_test);
        spinner_view_->addItem(L"Test01");
        spinner_view_->addItem(L"Test02");
        spinner_view_->addItem(L"Test03");

        auto textView = findViewById<ukive::TextView>(Res::Id::tv_test_txt);
        textView->setText(L"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii这是一个示例程序，\n\n在这里可以显示文本。\n这是一个示例程序，\n在这里可以显示文本。\n这是一个示例程序，\n在这里可以显示文本。");
        textView->setBackground(new ukive::ColorDrawable(ukive::Color::White));

        ukive::UnderlineSpan* span = new ukive::UnderlineSpan(3, 5);
        textView->getEditable()->addSpan(span, ukive::Editable::Reason::API);

        image_view_ = findViewById<ukive::ImageView>(Res::Id::iv_test_img);
        std::wstring imgFileName = ukive::Application::getExecFileName(true);
        auto bitmap = ukive::BitmapFactory::decodeFile(this, ukive::File(imgFileName, L"freshpaint.png").getPath());
        image_view_->setImageBitmap(bitmap);

        test_button_ = findViewById<ukive::Button>(Res::Id::bt_test_button);
        test_button_->setOnClickListener(this);
    }

    void TestWindow::inflateListView() {
        setContentView(Res::Layout::test_window_list_layout_xml);

        // Buttons
        dwm_button_ = findViewById<ukive::Button>(Res::Id::bt_dwm_button);
        dwm_button_->setOnClickListener(this);

        // ListView
        auto adapter = new TestAdapter();
        for (int i = 0; i < 36; ++i) {
            adapter->AddItem(0, L"test", L"test test");
        }

        auto list_view = findViewById<ukive::ListView>(Res::Id::lv_test_list);
        list_view->setLayouter(new ukive::GridListLayouter());
        list_view->setAdapter(adapter);
    }

}
