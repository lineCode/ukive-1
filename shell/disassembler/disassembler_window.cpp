#include "shell/disassembler/disassembler_window.h"

#include "ukive/log.h"
#include "ukive/views/list/list_view.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/list/linear_list_layouter.h"
#include "ukive/views/button.h"

#include "intel_instructions.h"
#include "opcode_map.h"
#include "opcode_list_adapter.h"
#include "code_data_provider.h"
#include <iomanip>


namespace shell {

    using namespace dpr;

    DisassemblerWindow::DisassemblerWindow()
        : continue_btn_(nullptr),
          op_list_view_(nullptr),
          op_list_adapter_(nullptr) {
    }

    void DisassemblerWindow::onCreate() {
        Window::onCreate();

        using Rlp = ukive::RestraintLayoutParams;
        auto layout = new ukive::RestraintLayout(this);
        layout->setLayoutParams(
            new ukive::LayoutParams(
            ukive::LayoutParams::MATCH_PARENT,
            ukive::LayoutParams::MATCH_PARENT));
        setContentView(layout);

        continue_btn_ = new ukive::Button(this);
        continue_btn_->setEnabled(false);
        continue_btn_->setOnClickListener(this);
        continue_btn_->setText(L"Continue");
        auto con_btn_lp = Rlp::Builder(Rlp::FIT_CONTENT, Rlp::FIT_CONTENT)
            .start(layout->getId(), Rlp::START, dpToPxX(8))
            .top(layout->getId(), Rlp::TOP, dpToPxX(8)).build();
        layout->addView(continue_btn_, con_btn_lp);

        op_list_view_ = new ukive::ListView(this);
        op_list_view_->setLayouter(new ukive::LinearListLayouter());
        op_list_adapter_ = new OpcodeListAdapter();
        op_list_view_->setAdapter(op_list_adapter_);

        auto olv_lp = Rlp::Builder(Rlp::MATCH_PARENT, Rlp::MATCH_PARENT)
            .start(layout->getId(), Rlp::START, dpToPxX(8))
            .end(layout->getId(), Rlp::END, dpToPxX(8))
            .top(continue_btn_->getId(), Rlp::BOTTOM, dpToPxX(8))
            .bottom(layout->getId(), Rlp::BOTTOM, dpToPxX(8)).build();
        layout->addView(op_list_view_, olv_lp);

        showTitleBar();

        initOneByteOpcodeMap();
        initTwoByteOpcodeMap();
        initThreeByteOpcodeMap();
        initExtensionOpcodeMap();
        initModRMMap();
        initSIBMap();

        debugger_.setDebuggerBridge(this);
        debugger_.create(L"D:\\test.exe");
    }

    void DisassemblerWindow::onClick(ukive::View* v) {
        if (v == continue_btn_) {
            continue_btn_->setEnabled(false);
            debugger_.resume();
        }
    }

    void DisassemblerWindow::onBreakpoint(const DebugInfo& info) {
        /*processStaticInstructions(
            reinterpret_cast<uint8_t*>(info.sec_base_addr),
            info.bp_addr - info.sec_base_addr,
            info.sec_size);*/
        processDynamicInstructions(info);
        continue_btn_->setEnabled(true);
        //debugger_.resume();
    }

    void DisassemblerWindow::onSingleStep(const DebugInfo& info) {
        /*processStaticInstructions(
            reinterpret_cast<uint8_t*>(info.sec_base_addr),
            info.bp_addr - info.sec_base_addr,
            info.sec_size);*/
        processDynamicInstructions(info);
        continue_btn_->setEnabled(true);
        //debugger_.resume();
    }

    void DisassemblerWindow::processStaticInstructions(const uint8_t* buf, uint32_t ep, uint32_t size) {
        ExtraInfo info;

        CodeSegInfo csi;
        csi.provider = new StaticCodeDataProvider(buf);
        csi.cur = ep;
        csi.size = size;
        csi.cpu_mode = CPUMode::_32Bit;

        for (int i = 0; i < 50; ++i) {
            if (parseInstruction(csi, &info)) {
                csi += info.length();
                info.reset();
            } else {
                break;
            }
        }

        delete csi.provider;
    }

    void DisassemblerWindow::processDynamicInstructions(const DebugInfo& dbg_info) {
        ExtraInfo info;

        op_list_adapter_->clear();

        CodeSegInfo csi;
        csi.provider = new DynamicCodeDataProvider(dbg_info.sec_base_addr, dbg_info.process);
        csi.cur = dbg_info.bp_addr - dbg_info.sec_base_addr;
        csi.size = dbg_info.sec_size;
        csi.cpu_mode = CPUMode::_32Bit;

        uint32_t offset = 0;
        for (int i = 0; i < 50; ++i) {
            std::wstringstream addr_ss;
            addr_ss << std::hex << std::uppercase << std::setw(8) << std::setfill(L'0')
                << (dbg_info.bp_addr + offset);
            auto addr_str = addr_ss.str();

            if (parseInstruction(csi, &info)) {
                op_list_adapter_->addOpcode(addr_str, ukive::UTF8ToUTF16(info.toString()));
                csi += info.length();
                offset += info.length();
                info.reset();
            } else {
                op_list_adapter_->addOpcode(addr_str, L"Unknown!!!");
                break;
            }
        }

        delete csi.provider;
    }

}
