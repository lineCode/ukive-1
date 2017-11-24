#ifndef SHELL_TEST_ANIMATION_WALLPAPER_TRANSFORM_H_
#define SHELL_TEST_ANIMATION_WALLPAPER_TRANSFORM_H_

#include <d2d1effectauthor.h>
#include <d2d1effecthelpers.h>

// Example GUID used to uniquely identify HLSL shader. Passed to Direct2D during
// shader load, and used by the transform to identify the shader for the
// ID2D1DrawInfo::SetPixelShader method. The effect author should create a
// unique name for the shader as well as a unique GUID using a GUID generation tool.
// {C6EDFB3F-D8FB-40CC-816C-979911216742}
DEFINE_GUID(GUID_UWallpaperPixelShader, 0xc6edfb3f, 0xd8fb, 0x40cc, 0x81, 0x6c, 0x97, 0x99, 0x11, 0x21, 0x67, 0x42);


namespace shell {

    class WallpaperTransform : public ID2D1DrawTransform
    {
    private:
        LONG mRefCount;
        D2D1_RECT_L mInputRect;

        // ÏñËØ×ÅÉ«Æ÷µÄConstantBuffer¡£
        struct
        {
            float bound[4];
        } mConstantBuffer;

    public:
        WallpaperTransform();
        ~WallpaperTransform();

        IFACEMETHODIMP_(UINT32) GetInputCount() const override;

        IFACEMETHODIMP MapInputRectsToOutputRect(
            _In_reads_(inputRectCount) const D2D1_RECT_L* pInputRects,
            _In_reads_(inputRectCount) const D2D1_RECT_L* pInputOpaqueSubRects,
            UINT32 inputRectCount,
            _Out_ D2D1_RECT_L* pOutputRect,
            _Out_ D2D1_RECT_L* pOutputOpaqueSubRect
        ) override;

        IFACEMETHODIMP MapOutputRectToInputRects(
            _In_ const D2D1_RECT_L* pOutputRect,
            _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
            UINT32 inputRectCount
        ) const override;

        IFACEMETHODIMP MapInvalidRect(
            UINT32 inputIndex,
            D2D1_RECT_L invalidInputRect,
            _Out_ D2D1_RECT_L* pInvalidOutputRect
        ) const override;

        IFACEMETHODIMP SetDrawInfo(
            ID2D1DrawInfo *drawInfo
        ) override;

        IFACEMETHODIMP_(ULONG) AddRef() override;
        IFACEMETHODIMP_(ULONG) Release() override;
        IFACEMETHODIMP QueryInterface(REFIID riid, _Outptr_ void** ppOutput) override;
    };

}

#endif  // SHELL_TEST_ANIMATION_WALLPAPER_TRANSFORM_H_