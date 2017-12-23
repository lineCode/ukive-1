﻿#include "shape_drawable.h"

#include "ukive/graphics/canvas.h"
#include "ukive/graphics/rect.h"


namespace ukive {

    ShapeDrawable::ShapeDrawable(int shape)
        :Drawable(),
        mShape(shape)
    {
        mWidth = -1;
        mHeight = -1;

        mHasSolid = false;
        mHasStroke = false;
        mRoundRadius = 1.f;
        mStrokeWidth = 1.f;

        mSolidColor = Color(0, 0.f);
        mStrokeColor = Color(0, 0.f);
    }


    ShapeDrawable::~ShapeDrawable()
    {
    }


    void ShapeDrawable::setSize(int width, int height)
    {
        mWidth = width;
        mHeight = height;
    }

    void ShapeDrawable::setRadius(float radius)
    {
        mRoundRadius = radius;
    }

    void ShapeDrawable::setSolidEnable(bool enable)
    {
        mHasSolid = enable;
    }

    void ShapeDrawable::setSolidColor(Color color)
    {
        mSolidColor = color;
    }

    void ShapeDrawable::setStrokeEnable(bool enable)
    {
        mHasStroke = enable;
    }

    void ShapeDrawable::setStrokeWidth(float width)
    {
        mStrokeWidth = width;
    }

    void ShapeDrawable::setStrokeColor(Color color)
    {
        mStrokeColor = color;
    }


    void ShapeDrawable::draw(Canvas *canvas)
    {
        if (!mHasSolid && !mHasStroke)
            return;

        RectF bound;
        if (mWidth == -1 || mHeight == -1)
            bound = getBounds();
        else
            bound = RectF(0, 0, mWidth, mHeight);

        switch (mShape)
        {
        case SHAPE_RECT:
        {
            if (mHasSolid)
                canvas->fillRect(bound, mSolidColor);
            if (mHasStroke)
                canvas->drawRect(bound, mStrokeColor);
            break;
        }
        case SHAPE_ROUND_RECT:
        {
            if (mHasSolid)
                canvas->fillRoundRect(bound, mRoundRadius, mSolidColor);
            if (mHasStroke)
                canvas->drawRoundRect(bound, mStrokeWidth, mRoundRadius, mStrokeColor);
            break;
        }
        case SHAPE_OVAL:
        {
            float cx = (bound.right - bound.left) / 2.f;
            float cy = (bound.bottom - bound.top) / 2.f;

            if (mHasSolid)
                canvas->fillOval(
                    cx, cy, cx, cy, mSolidColor);
            if (mHasStroke)
                canvas->drawOval(
                    cx, cy, cx, cy, mStrokeColor);
            break;
        }
        }
    }

    float ShapeDrawable::getOpacity()
    {
        if ((mHasSolid && mSolidColor.a != 0.f)
            || (mHasStroke && mStrokeColor.a != 0.f && mStrokeWidth != 0.f))
            return 1.f;
        else
            return 0.f;
    }

    int ShapeDrawable::getIncWidth()
    {
        return mWidth;
    }

    int ShapeDrawable::getIncHeight()
    {
        return mHeight;
    }

}