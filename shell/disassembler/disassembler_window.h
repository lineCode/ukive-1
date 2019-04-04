#ifndef DISASSEMBLER_DECOMPILER_WINDOW_H_
#define DISASSEMBLER_DECOMPILER_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/views/click_listener.h"

#include "pe_file.h"
#include "debugger.h"


namespace ukive {

    class Button;
    class ListView;

}

namespace shell {

    class OpcodeListAdapter;

    class DisassemblerWindow :
        public ukive::Window,
        public ukive::OnClickListener,
        public dpr::DebuggerBridge {
    public:
        DisassemblerWindow();

        // ukive::Window
        void onCreate() override;

        // ukive::OnClickListener
        void onClick(ukive::View* v) override;

        // dpr::DebuggerBridge
        void onBreakpoint(const DebugInfo& info) override;
        void onSingleStep(const DebugInfo& info) override;

    private:
        void processStaticInstructions(const uint8_t* buf, uint32_t ep, uint32_t size);
        void processDynamicInstructions(const DebugInfo& dbg_info);

        dpr::Debugger debugger_;
        ukive::Button* continue_btn_;
        ukive::ListView* op_list_view_;
        OpcodeListAdapter* op_list_adapter_;
    };

}

#endif  // DISASSEMBLER_DECOMPILER_WINDOW_H_