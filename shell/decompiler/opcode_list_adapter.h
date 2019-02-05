#ifndef DECOMPILER_OPCODE_LIST_ADAPTER_H_
#define DECOMPILER_OPCODE_LIST_ADAPTER_H_

#include "ukive/views/list/list_adapter.h"


namespace ukive {

    class TextView;

}

namespace shell {

    class OpcodeListAdapter : public ukive::ListAdapter {
    public:
        enum {
            ID_OPCODE_ADDRESS = 1,
            ID_OPCODE_TEXT,
        };

        struct BindData {
            string16 addr;
            string16 opcode;
        };

        class OpcodeViewHolder : public ViewHolder {
        public:
            explicit OpcodeViewHolder(ukive::View* v)
                : ViewHolder(v),
                  addr_tv(nullptr),
                  opcode_tv(nullptr)
            {
                addr_tv = reinterpret_cast<ukive::TextView*>(v->findViewById(ID_OPCODE_ADDRESS));
                opcode_tv = reinterpret_cast<ukive::TextView*>(v->findViewById(ID_OPCODE_TEXT));
            }

            ukive::TextView* addr_tv;
            ukive::TextView* opcode_tv;
        };

        OpcodeListAdapter();

        void addOpcode(const string16& addr, const string16& op);
        void clear();

    protected:
        // ukive::ListAdapter
        ViewHolder* onCreateViewHolder(ukive::ViewGroup* parent, int position) override;
        void onBindViewHolder(ViewHolder* holder, int position) override;
        int getItemCount() override;

    private:
        std::vector<BindData> data_;
    };

}

#endif  // DECOMPILER_OPCODE_LIST_ADAPTER_H_