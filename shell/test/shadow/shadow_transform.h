#ifndef SHELL_TEST_SHADOW_SHADOW_TRANSFORM_H_
#define SHELL_TEST_SHADOW_SHADOW_TRANSFORM_H_

#include <d2d1effectauthor.h>
#include <d2d1effecthelpers.h>

#include "ukive/utils/com_ptr.h"

#include "shadow_effect.h"


// Example GUID used to uniquely identify HLSL shader. Passed to Direct2D during
// shader load, and used by the transform to identify the shader for the
// ID2D1DrawInfo::SetPixelShader method. The effect author should create a
// unique name for the shader as well as a unique GUID using a GUID generation tool.
// {62836AE3-5808-46D4-B27A-FC4DB7FEFDEC}
DEFINE_GUID(GUID_ShadowPixelShader, 0x62836AE3, 0x5808, 0x46D4, 0xB2, 0x7A, 0xFC, 0x4D, 0xB7, 0xFE, 0xFD, 0xEC);


namespace shell {

    class ShadowTransform : public ID2D1DrawTransform
    {
    public:
        ShadowTransform();
        ~ShadowTransform();

        HRESULT SetAlpha(float alpha);
        HRESULT SetShape(SHADOW_SHAPE shape);
        HRESULT SetBounds(D2D_VECTOR_4F bounds);
        HRESULT SetOffset(D2D_VECTOR_2F offset);
        HRESULT SetElevation(float elevation);
        HRESULT SetCornerRadius(float radius);
        HRESULT SetTexture(ID2D1ResourceTexture* texture, UINT32 extent);

        IFACEMETHODIMP_(UINT32) GetInputCount() const override;

        IFACEMETHODIMP MapInputRectsToOutputRect(
            _In_reads_(inputRectCount) const D2D1_RECT_L* pInputRects,
            _In_reads_(inputRectCount) const D2D1_RECT_L* pInputOpaqueSubRects,
            UINT32 inputRectCount,
            _Out_ D2D1_RECT_L* pOutputRect,
            _Out_ D2D1_RECT_L* pOutputOpaqueSubRect) override;

        IFACEMETHODIMP MapOutputRectToInputRects(
            _In_ const D2D1_RECT_L* pOutputRect,
            _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
            UINT32 inputRectCount) const override;

        IFACEMETHODIMP MapInvalidRect(
            UINT32 inputIndex,
            D2D1_RECT_L invalidInputRect,
            _Out_ D2D1_RECT_L* pInvalidOutputRect) const override;

        IFACEMETHODIMP SetDrawInfo(
            ID2D1DrawInfo* drawInfo) override;

        IFACEMETHODIMP_(ULONG) AddRef() override;
        IFACEMETHODIMP_(ULONG) Release() override;
        IFACEMETHODIMP QueryInterface(REFIID riid, _Outptr_ void** ppOutput) override;

    private:
        ULONG ref_count_;

        LONG expanded_;
        D2D1_RECT_L input_rect_;
        ID2D1DrawInfo* draw_info_;
        UINT32 texture_extent_;

        // ÏñËØ×ÅÉ«Æ÷µÄ ConstantBuffer¡£
        struct {
            float bounds[4];
            float offset[2];
            float alpha;
            float elevation;
            float corner_radius;
        } const_buffer_;
    };

}

#endif  // SHELL_TEST_ANIMATION_WALLPAPER_TRANSFORM_H_