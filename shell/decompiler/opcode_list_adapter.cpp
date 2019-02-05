#include "opcode_list_adapter.h"

#include "ukive/utils/stl_utils.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/text_view.h"
#include "ukive/window/window.h"


namespace shell {

    OpcodeListAdapter::OpcodeListAdapter() {
    }

    void OpcodeListAdapter::addOpcode(const string16& addr, const string16& op) {
        BindData dat;
        dat.addr = addr;
        dat.opcode = op;
        data_.push_back(dat);
        notifyDataChanged();
    }

    void OpcodeListAdapter::clear() {
        data_.clear();
        notifyDataChanged();
    }

    // ukive::ListAdapter
    ukive::ListAdapter::ViewHolder* OpcodeListAdapter::onCreateViewHolder(
        ukive::ViewGroup* parent, int position)
    {
        using Rlp = ukive::RestraintLayoutParams;
        auto layout = new ukive::RestraintLayout(parent->getWindow());
        layout->setLayoutParams(
            new ukive::LayoutParams(
                ukive::LayoutParams::MATCH_PARENT,
                ukive::LayoutParams::FIT_CONTENT));

        auto op_addr_view = new ukive::TextView(parent->getWindow());
        op_addr_view->setId(ID_OPCODE_ADDRESS);
        op_addr_view->setFontFamilyName(L"Consolas");
        auto op_ad_lp = Rlp::Builder(Rlp::FIT_CONTENT, Rlp::FIT_CONTENT)
            .start(layout->getId())
            .top(layout->getId()).bottom(layout->getId()).build();
        layout->addView(op_addr_view, op_ad_lp);

        auto op_text_view = new ukive::TextView(parent->getWindow());
        op_text_view->setId(ID_OPCODE_TEXT);
        op_text_view->setFontFamilyName(L"Consolas");
        auto op_tv_lp = Rlp::Builder(Rlp::FIT_CONTENT, Rlp::FIT_CONTENT)
            .start(op_addr_view->getId(), Rlp::END, parent->getWindow()->dpToPx(16))
            .top(layout->getId()).bottom(layout->getId()).build();
        layout->addView(op_text_view, op_tv_lp);

        return new OpcodeViewHolder(layout);
    }

    void OpcodeListAdapter::onBindViewHolder(ViewHolder* holder, int position) {
        auto op_holder = reinterpret_cast<OpcodeViewHolder*>(holder);
        op_holder->addr_tv->setText(data_[position].addr);
        op_holder->opcode_tv->setText(data_[position].opcode);
    }

    int OpcodeListAdapter::getItemCount() {
        return ukive::STLCInt(data_.size());
    }

}
