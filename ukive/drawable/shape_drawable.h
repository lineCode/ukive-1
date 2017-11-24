#ifndef UKIVE_DRAWABLE_SHAPE_DRAWABLE_H_
#define UKIVE_DRAWABLE_SHAPE_DRAWABLE_H_

#include "ukive/drawable/drawable.h"


namespace ukive {

    class ShapeDrawable : public Drawable
    {
    private:
        int mWidth;
        int mHeight;
        int mShape;

        bool mHasSolid;
        bool mHasStroke;
        float mRoundRadius;
        float mStrokeWidth;
        D2D1_COLOR_F mSolidColor;
        D2D1_COLOR_F mStrokeColor;

    public:
        static const int SHAPE_RECT = 0;
        static const int SHAPE_ROUND_RECT = 1;
        static const int SHAPE_OVAL = 2;

    public:
        ShapeDrawable(int shape);
        ~ShapeDrawable();

        void setSize(int width, int height);
        void setRadius(float radius);

        void setSolidEnable(bool enable);
        void setSolidColor(D2D1_COLOR_F color);
        void setStrokeEnable(bool enable);
        void setStrokeWidth(float width);
        void setStrokeColor(D2D1_COLOR_F color);

        virtual void draw(Canvas *canvas);

        virtual float getOpacity();

        virtual int getIncWidth();
        virtual int getIncHeight();
    };

}

#endif  // UKIVE_DRAWABLE_SHAPE_DRAWABLE_H_