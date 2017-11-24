#include "wallpaper_transform.h"

#include "ukive/utils/hresult_utils.h"


namespace shell {

    WallpaperTransform::WallpaperTransform()
        :mRefCount(1)
    {
        mInputRect.left = 100;
        mInputRect.top = 100;
        mInputRect.right = 200;
        mInputRect.bottom = 200;

        mConstantBuffer.bound[0] = mInputRect.left;
        mConstantBuffer.bound[1] = mInputRect.top;
        mConstantBuffer.bound[2] = mInputRect.right;
        mConstantBuffer.bound[3] = mInputRect.bottom;
    }


    WallpaperTransform::~WallpaperTransform()
    {
    }


    IFACEMETHODIMP_(UINT32) WallpaperTransform::GetInputCount() const
    {
        return 0;
    }

    IFACEMETHODIMP WallpaperTransform::MapInputRectsToOutputRect(
        _In_reads_(inputRectCount) const D2D1_RECT_L* pInputRects,
        _In_reads_(inputRectCount) const D2D1_RECT_L* pInputOpaqueSubRects,
        UINT32 inputRectCount,
        _Out_ D2D1_RECT_L* pOutputRect,
        _Out_ D2D1_RECT_L* pOutputOpaqueSubRect
    )
    {
        // 该变换没有输入。
        if (inputRectCount != 0)
            return E_INVALIDARG;

        *pOutputRect = mInputRect;
        // Indicate that the image's opacity has not changed.
        *pOutputOpaqueSubRect = pInputOpaqueSubRects[0];

        return S_OK;
    }

    IFACEMETHODIMP WallpaperTransform::MapOutputRectToInputRects(
        _In_ const D2D1_RECT_L* pOutputRect,
        _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
        UINT32 inputRectCount
    ) const
    {
        // 该变换没有输入。
        if (inputRectCount != 0)
            return E_INVALIDARG;

        // The input needed for the transform is the same as the visible output.
        if (pInputRects != nullptr)
            pInputRects[0] = *pOutputRect;

        return S_OK;
    }

    IFACEMETHODIMP WallpaperTransform::MapInvalidRect(
        UINT32 inputIndex,
        D2D1_RECT_L invalidInputRect,
        _Out_ D2D1_RECT_L* pInvalidOutputRect
    ) const
    {
        // If part of the transform's input is invalid, mark the corresponding
        // output region as invalid.
        *pInvalidOutputRect = invalidInputRect;

        return S_OK;
    }

    IFACEMETHODIMP WallpaperTransform::SetDrawInfo(
        ID2D1DrawInfo *drawInfo
    )
    {
        RH(drawInfo->SetPixelShader(GUID_UWallpaperPixelShader));
        RH(drawInfo->SetPixelShaderConstantBuffer((BYTE*)&mConstantBuffer, sizeof(mConstantBuffer)));

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) WallpaperTransform::AddRef()
    {
        return InterlockedIncrement(&mRefCount);
    }

    IFACEMETHODIMP_(ULONG) WallpaperTransform::Release()
    {
        LONG cr = InterlockedDecrement(&mRefCount);

        if (mRefCount == 0)
            delete this;

        return cr;
    }

    IFACEMETHODIMP WallpaperTransform::QueryInterface(REFIID riid, _Outptr_ void** ppOutput)
    {
        if (ppOutput == nullptr)
            return E_INVALIDARG;

        *ppOutput = nullptr;

        if (IsEqualIID(riid, IID_IUnknown))
        {
            *ppOutput = reinterpret_cast<IUnknown*>(this);
        }
        else if (IsEqualIID(riid, __uuidof(ID2D1Transform)))
        {
            *ppOutput = (ID2D1Transform*)this;
        }
        else if (IsEqualIID(riid, __uuidof(ID2D1DrawTransform)))
        {
            *ppOutput = (ID2D1DrawTransform*)this;
        }

        if (*ppOutput)
        {
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

}