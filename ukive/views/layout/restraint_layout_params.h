#ifndef UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_PARAMS_H_
#define UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_PARAMS_H_

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    class RestraintLayoutParams : public LayoutParams {
    public:
        enum Edge {
            START = 0,
            TOP,
            END,
            BOTTOM,
            NONE
        };

        enum CoupleHandleType {
            CH_FILL = 0,
            CH_WRAP = 1,
            CH_FIXED = 2,
        };

        RestraintLayoutParams();
        RestraintLayoutParams(int width, int height);
        explicit RestraintLayoutParams(const LayoutParams &lp);
        ~RestraintLayoutParams();

        // ������һ�β������������ LayoutParams �󶨵� View
        // �Ĳ�����Ϣ��ע��ǵ��ڲ������̿�ʼǰ����ñ�ǡ�
        int spec_width;
        int spec_width_mode;
        int spec_height;
        int spec_height_mode;
        bool is_width_measured;
        bool is_height_measured;

        // ������һ�β��ֹ�������� LayoutParams �󶨵� View
        // �Ĳ�����Ϣ��ע��ǵ��ڲ��ֹ��̿�ʼǰ����ñ�ǡ�
        int left, right;
        int top, bottom;
        bool is_vert_layouted;
        bool is_hori_layouted;

        int vertical_weight;
        int horizontal_weight;

        float vertical_percent;
        float horizontal_percent;

        CoupleHandleType hori_couple_handler_type;
        CoupleHandleType vert_couple_handler_type;

        int start_handled_id;
        Edge start_handled_edge;
        int top_handled_id;
        Edge top_handled_edge;
        int end_handled_id;
        Edge end_handled_edge;
        int bottom_handled_id;
        Edge bottom_handled_edge;

        RestraintLayoutParams* startHandle(int handle_id, Edge handle_edge, int margin = 0);
        RestraintLayoutParams* topHandle(int handle_id, Edge handle_edge, int margin = 0);
        RestraintLayoutParams* endHandle(int handle_id, Edge handle_edge, int margin = 0);
        RestraintLayoutParams* bottomHandle(int handle_id, Edge handle_edge, int margin = 0);

    public:
        class Builder {
        public:
            Builder()
                : built_(false), lp_(new RestraintLayoutParams()) {}
            Builder(int width, int height)
                : built_(false), lp_(new RestraintLayoutParams(width, height)) {}
            ~Builder() { if (!built_) delete lp_; }

            Builder& start(int handle_id, Edge handle_edge = START, int margin = 0) {
                lp_->startHandle(handle_id, handle_edge, margin);
                return *this;
            }

            Builder& top(int handle_id, Edge handle_edge = TOP, int margin = 0) {
                lp_->topHandle(handle_id, handle_edge, margin);
                return *this;
            }

            Builder& end(int handle_id, Edge handle_edge = END, int margin = 0) {
                lp_->endHandle(handle_id, handle_edge, margin);
                return *this;
            }

            Builder& bottom(int handle_id, Edge handle_edge = BOTTOM, int margin = 0) {
                lp_->bottomHandle(handle_id, handle_edge, margin);
                return *this;
            }

            RestraintLayoutParams* build() {
                built_ = true;
                return lp_;
            }

        private:
            bool built_;
            RestraintLayoutParams* lp_;
        };

        bool hasStart();
        bool hasTop();
        bool hasEnd();
        bool hasBottom();

        bool hasVerticalWeight();
        bool hasHorizontalWeight();

        bool hasVerticalCouple();
        bool hasHorizontalCouple();

    private:
        void initLayoutParams();
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_PARAMS_H_