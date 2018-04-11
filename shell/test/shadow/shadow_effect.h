#ifndef SHELL_TEST_SHADOW_SHADOW_EFFECT_H_
#define SHELL_TEST_SHADOW_SHADOW_EFFECT_H_

#include <initguid.h>
#include <d2d1effects_1.h>
#include <d2d1effectauthor.h>
#include <d2d1effecthelpers.h>

#include "ukive/utils/com_ptr.h"


namespace shell {

    class ShadowTransform;

    enum SHADOW_EFFECT_PROP {
        SHADOW_EFFECT_PROP_ALPHA = 0,
        SHADOW_EFFECT_PROP_SHAPE,
        SHADOW_EFFECT_PROP_BOUNDS,
        SHADOW_EFFECT_PROP_OFFSET,
        SHADOW_EFFECT_PROP_ELEVATION,
        SHADOW_EFFECT_PROP_CORNER_RADIUS
    };

    typedef enum {
        SHADOW_OVAL = 0,
        SHADOW_SHAPE_RECT,
        SHADOW_SHAPE_ROUNDRECT
    } SHADOW_SHAPE;

    // 用于唯一标识该效果的GUID。它被用于向Direct2D注册该效果，
    // 和在开发者调用 ID2D1DeviceContext::CreateEffect 方法时
    // 标识该效果。该效果的名称在整个应用中必须唯一，GUID也一样。
    // {52C7FB7A-C634-4036-B061-BDBA15B6152C}
    DEFINE_GUID(CLSID_ShadowEffect, 0x52C7FB7A, 0xC634, 0x4036, 0xB0, 0x61, 0xBD, 0xBA, 0x15, 0xB6, 0x15, 0x2C);


    class ShadowEffect : public ID2D1EffectImpl
    {
    public:
        // 在应用调用 ID2D1DeviceContext::CreateEffect 后，Direct2D
        // 会调用此方法。你可以在此方法中进行内部初始化或其他与效果有关的工作。
        // 另外，你还可以在此创建初始变换图形。
        IFACEMETHODIMP Initialize(
            _In_ ID2D1EffectContext* pContextInternal,
            _In_ ID2D1TransformGraph* pTransformGraph);

        IFACEMETHODIMP PrepareForRender(D2D1_CHANGE_TYPE changeType);
        IFACEMETHODIMP SetGraph(_In_ ID2D1TransformGraph* pGraph);

        static HRESULT __stdcall Register(_In_ ID2D1Factory1* pFactory);
        static HRESULT __stdcall CreateEffect(_Outptr_ IUnknown** ppEffectImpl);

        HRESULT SetAlpha(float alpha);
        HRESULT SetShape(SHADOW_SHAPE shape);
        HRESULT SetBounds(D2D_VECTOR_4F bounds);
        HRESULT SetOffset(D2D_VECTOR_2F offset);
        HRESULT SetElevation(float elevation);
        HRESULT SetCornerRadius(float radius);

        float GetAlpha() const;
        SHADOW_SHAPE GetShape() const;
        D2D_VECTOR_4F GetBounds() const;
        D2D_VECTOR_2F GetOffset() const;
        float GetElevation() const;
        float GetCornerRadius() const;

        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput);

    private:
        // 构造函数必须私有以保证其不被外界调用。
        ShadowEffect();

        HRESULT CreateGaussStencil1D(ID2D1EffectContext* ec);
        HRESULT CreateGaussStencil2D(ID2D1EffectContext* ec);

        ULONG ref_count_;
        ukive::ComPtr<ShadowTransform> transform_;
        ukive::ComPtr<ID2D1ResourceTexture> texture_;

        float alpha_;
        float elevation_;
        float corner_radius_;
        SHADOW_SHAPE shape_;
        D2D_VECTOR_4F bounds_;
        D2D_VECTOR_2F offset_;
    };

}

#endif  // SHELL_TEST_SHADOW_SHADOW_EFFECT_H_