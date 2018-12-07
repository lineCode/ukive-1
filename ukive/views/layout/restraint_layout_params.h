#ifndef UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_PARAMS_H_
#define UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_PARAMS_H_

#include "ukive/views/layout/layout_params.h"


namespace ukive {

    class RestraintLayoutParams : public LayoutParams
    {
    private:
        void initLayoutParams();

    public:
        enum Edge {
            START = 0,
            TOP,
            END,
            BOTTOM,
            NONE
        };

        static const int CH_FILL = 0;
        static const int CH_WRAP = 1;
        static const int CH_FIXED = 2;

    public:
        RestraintLayoutParams();
        RestraintLayoutParams(int width, int height);
        RestraintLayoutParams(const LayoutParams &lp);
        ~RestraintLayoutParams();

        // ������һ�β������������ LayoutParams �󶨵� View
        // �Ĳ�����Ϣ��ע��ǵ��ڲ������̿�ʼǰ����ñ�ǡ�
        int specWidth;
        int specWidthMode;
        int specHeight;
        int specHeightMode;
        bool isWidthMeasured;
        bool isHeightMeasured;

        // ������һ�β��ֹ�������� LayoutParams �󶨵� View
        // �Ĳ�����Ϣ��ע��ǵ��ڲ��ֹ��̿�ʼǰ����ñ�ǡ�
        int left, right;
        int top, bottom;
        bool isVertLayouted;
        bool isHoriLayouted;

        int vertical_weight;
        int horizontal_weight;

        float verticalPercent;
        float horizontalPercent;

        int horiCoupleHandlerType;
        int vertCoupleHandlerType;

        int startHandledId;
        int startHandledEdge;
        int topHandledId;
        int topHandledEdge;
        int endHandledId;
        int endHandledEdge;
        int bottomHandledId;
        int bottomHandledEdge;

        RestraintLayoutParams* startHandle(int handleId, int handleEdge, int margin = 0);
        RestraintLayoutParams* topHandle(int handleId, int handleEdge, int margin = 0);
        RestraintLayoutParams* endHandle(int handleId, int handleEdge, int margin = 0);
        RestraintLayoutParams* bottomHandle(int handleId, int handleEdge, int margin = 0);

    public:
        class Builder
        {
        public:
            Builder()
                :built_(false), lp_(new RestraintLayoutParams()) {}
            Builder(int width, int height)
                :built_(false), lp_(new RestraintLayoutParams(width, height)) {}
            ~Builder() { if (!built_)delete lp_; }

            Builder& start(int handleId, Edge handleEdge = START, int margin = 0) {
                lp_->startHandle(handleId, handleEdge, margin);
                return *this;
            }

            Builder& top(int handleId, Edge handleEdge = TOP, int margin = 0) {
                lp_->topHandle(handleId, handleEdge, margin);
                return *this;
            }

            Builder& end(int handleId, Edge handleEdge = END, int margin = 0) {
                lp_->endHandle(handleId, handleEdge, margin);
                return *this;
            }

            Builder& bottom(int handleId, Edge handleEdge = BOTTOM, int margin = 0) {
                lp_->bottomHandle(handleId, handleEdge, margin);
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
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_PARAMS_H_