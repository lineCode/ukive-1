#include "test_adapter.h"

#include <sstream>

#include "ukive/log.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/image_view.h"
#include "ukive/views/text_view.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/graphics/color.h"
#include "ukive/utils/stl_utils.h"


namespace shell {

    ukive::ListAdapter::ViewHolder* TestAdapter::onCreateViewHolder(
        ukive::ViewGroup* parent, int position) {

        auto layout = new ukive::RestraintLayout(parent->getWindow());
        layout->setBackground(new ukive::ColorDrawable(ukive::Color::Blue100));
        layout->setLayoutParams(
            new ukive::LayoutParams(ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::FIT_CONTENT));

        auto layout_lp = layout->getLayoutParams();
        layout_lp->left_margin = 4;
        layout_lp->top_margin = 4;
        layout_lp->right_margin = 4;
        layout_lp->bottom_margin = 4;

        using Rlp = ukive::RestraintLayoutParams;

        auto avatar_view = new ukive::ImageView(parent->getWindow());
        avatar_view->setId(ID_AVATAR);
        Rlp* av_lp = new Rlp(36, 36);
        av_lp->startHandle(layout->getId(), Rlp::START, 16);
        av_lp->topHandle(layout->getId(), Rlp::BOTTOM, 8);
        av_lp->bottomHandle(layout->getId(), Rlp::BOTTOM, 8);
        layout->addView(avatar_view, av_lp);

        ukive::TextView* title_label = new ukive::TextView(parent->getWindow());
        title_label->setId(ID_TITLE);
        Rlp* tl_lp = new Rlp(
            Rlp::MATCH_PARENT,
            Rlp::FIT_CONTENT);
        tl_lp->startHandle(avatar_view->getId(), Rlp::END, 8);
        tl_lp->topHandle(layout->getId(), Rlp::TOP, 8);
        tl_lp->endHandle(layout->getId(), Rlp::END, 8);
        layout->addView(title_label, tl_lp);

        ukive::TextView* summary_label = new ukive::TextView(parent->getWindow());
        summary_label->setId(ID_SUMMARY);
        Rlp* sl_lp = new Rlp(
            Rlp::MATCH_PARENT,
            Rlp::FIT_CONTENT);
        sl_lp->startHandle(avatar_view->getId(), Rlp::END, 8);
        sl_lp->topHandle(ID_TITLE, Rlp::BOTTOM, 4);
        sl_lp->endHandle(layout->getId(), Rlp::END, 8);
        sl_lp->bottomHandle(layout->getId(), Rlp::BOTTOM, 8);
        layout->addView(summary_label, sl_lp);

        return new TestViewHolder(layout);
    }

    void TestAdapter::onBindViewHolder(ViewHolder* holder, int position) {
        BindData* data = data_list_.at(position);
        TestViewHolder* test_holder = reinterpret_cast<TestViewHolder*>(holder);

        std::wstringstream ss;
        ss << data->title << " " << position;

        test_holder->title_label->setText(ss.str());
        test_holder->summary_label->setText(data->summary);

        //LOG(INFO) << "ListAdapter::OnBindViewHolder():" << position << " data has been bound.";
    }

    int TestAdapter::getItemCount() {
        return ukive::STLCInt(data_list_.size());
    }

    void TestAdapter::AddItem(int image_res_id, ukive::string16 title, ukive::string16 summary) {
        BindData* data = new BindData();
        data->image_resource_id = image_res_id;
        data->title = title;
        data->summary = summary;

        data_list_.push_back(data);
        notifyDataChanged();
    }

    void TestAdapter::AddItem(int pos, int image_res_id, ukive::string16 title, ukive::string16 summary) {
        BindData* data = new BindData();
        data->image_resource_id = image_res_id;
        data->title = title;
        data->summary = summary;

        if (data_list_.size() == 0)
            data_list_.push_back(data);
        else {
            int index = 0;
            for (auto it = data_list_.begin();
                it != data_list_.end(); ++it, ++index) {
                if (pos == index) {
                    data_list_.insert(it, data);
                    break;
                }
            }
        }

        notifyDataChanged();
    }

    void TestAdapter::ModifyItem(int image_res_id, ukive::string16 title, ukive::string16 summary) {
        for (auto it = data_list_.begin();
            it != data_list_.end(); ++it) {
            if ((*it)->title == title) {
                (*it)->image_resource_id = image_res_id;
                (*it)->summary = summary;
            }
        }

        notifyDataChanged();
    }

    void TestAdapter::RemoveItem(ukive::string16 title) {
        for (auto it = data_list_.begin();
            it != data_list_.end();) {
            if ((*it)->title == title) {
                it = data_list_.erase(it);
            } else {
                ++it;
            }
        }

        notifyDataChanged();
    }

    void TestAdapter::RemoveItem(int pos) {
        int index = 0;
        for (auto it = data_list_.begin();
            it != data_list_.end(); ++index, ++it) {
            if (index == pos) {
                data_list_.erase(it);
                break;
            }
        }

        notifyDataChanged();
    }

}