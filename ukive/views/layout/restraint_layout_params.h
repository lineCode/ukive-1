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
        RestraintLayoutParams(LayoutParams *lp);
        ~RestraintLayoutParams();

        //保存在一次测量过程中与该LayoutParams绑定的View
        //的测量信息。注意记得在测量过程开始前清除该标记。
        int specWidth;
        int specWidthMode;
        int specHeight;
        int specHeightMode;
        bool isWidthMeasured;
        bool isHeightMeasured;

        //保存在一次布局过程中与该LayoutParams绑定的View
        //的布局信息。注意记得在布局过程开始前清除该标记。
        int left, right;
        int top, bottom;
        bool isVertLayouted;
        bool isHoriLayouted;

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

            Builder& start(int margin = 0) {
                lp_->startHandle(-1, START, margin);
                return *this;
            }
            Builder& start(Edge handleEdge, int margin = 0) {
                lp_->startHandle(-1, handleEdge, margin);
                return *this;
            }
            Builder& start(int handleId, Edge handleEdge, int margin = 0) {
                lp_->startHandle(handleId, handleEdge, margin);
                return *this;
            }

            Builder& top(int margin = 0) {
                lp_->topHandle(-1, TOP, margin);
                return *this;
            }
            Builder& top(Edge handleEdge, int margin = 0) {
                lp_->topHandle(-1, handleEdge, margin);
                return *this;
            }
            Builder& top(int handleId, Edge handleEdge, int margin = 0) {
                lp_->topHandle(handleId, handleEdge, margin);
                return *this;
            }

            Builder& end(int margin = 0) {
                lp_->endHandle(-1, END, margin);
                return *this;
            }
            Builder& end(Edge handleEdge, int margin = 0) {
                lp_->endHandle(-1, handleEdge, margin);
                return *this;
            }
            Builder& end(int handleId, Edge handleEdge, int margin = 0) {
                lp_->endHandle(handleId, handleEdge, margin);
                return *this;
            }

            Builder& bottom(int margin = 0) {
                lp_->bottomHandle(-1, BOTTOM, margin);
                return *this;
            }
            Builder& bottom(Edge handleEdge, int margin = 0) {
                lp_->bottomHandle(-1, handleEdge, margin);
                return *this;
            }
            Builder& bottom(int handleId, Edge handleEdge, int margin = 0) {
                lp_->bottomHandle(handleId, handleEdge, margin);
                return *this;
            }

            RestraintLayoutParams* build() {
                built_ = true;
                return lp_;
            }

        private:
            bool built_;
            RestraintLayoutParams *lp_;
        };

        bool hasStart();
        bool hasTop();
        bool hasEnd();
        bool hasBottom();

        bool hasVerticalCouple();
        bool hasHorizontalCouple();
    };

}

#endif  // UKIVE_VIEWS_LAYOUT_RESTRAINT_LAYOUT_PARAMS_H_