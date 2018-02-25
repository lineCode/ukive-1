#include "shadow_transform.h"

#include "ukive/utils/hresult_utils.h"


namespace shell {

    ShadowTransform::ShadowTransform()
        :ref_count_(1) {

        // 单位为像素
        input_rect_.left = 0;      // x
        input_rect_.top = 0;       // y
        input_rect_.right = 220;    // z
        input_rect_.bottom = 100;   // w

        const_buffer_.bound[0] = input_rect_.left;
        const_buffer_.bound[1] = input_rect_.top;
        const_buffer_.bound[2] = input_rect_.right;
        const_buffer_.bound[3] = input_rect_.bottom;
    }


    ShadowTransform::~ShadowTransform() {
    }


    IFACEMETHODIMP_(UINT32) ShadowTransform::GetInputCount() const {
        return 0;
    }

    IFACEMETHODIMP ShadowTransform::MapInputRectsToOutputRect(
        _In_reads_(inputRectCount) const D2D1_RECT_L* pInputRects,
        _In_reads_(inputRectCount) const D2D1_RECT_L* pInputOpaqueSubRects,
        UINT32 inputRectCount,
        _Out_ D2D1_RECT_L* pOutputRect,
        _Out_ D2D1_RECT_L* pOutputOpaqueSubRect)
    {
        // 该变换没有输入。
        if (inputRectCount != 0) {
            return E_INVALIDARG;
        }

        *pOutputRect = input_rect_;
        // Indicate that the image's opacity has not changed.
        *pOutputOpaqueSubRect = pInputOpaqueSubRects[0];

        return S_OK;
    }

    IFACEMETHODIMP ShadowTransform::MapOutputRectToInputRects(
        _In_ const D2D1_RECT_L* pOutputRect,
        _Out_writes_(inputRectCount) D2D1_RECT_L* pInputRects,
        UINT32 inputRectCount) const
    {
        // 该变换没有输入。
        if (inputRectCount != 0) {
            return E_INVALIDARG;
        }

        // The input needed for the transform is the same as the visible output.
        if (pInputRects != nullptr) {
            pInputRects[0] = *pOutputRect;
        }

        return S_OK;
    }

    IFACEMETHODIMP ShadowTransform::MapInvalidRect(
        UINT32 inputIndex,
        D2D1_RECT_L invalidInputRect,
        _Out_ D2D1_RECT_L* pInvalidOutputRect) const
    {
        // If part of the transform's input is invalid, mark the corresponding
        // output region as invalid.
        *pInvalidOutputRect = invalidInputRect;

        return S_OK;
    }

    IFACEMETHODIMP ShadowTransform::SetDrawInfo(
        ID2D1DrawInfo* drawInfo)
    {
        RH(drawInfo->SetPixelShader(GUID_ShadowPixelShader));
        RH(drawInfo->SetPixelShaderConstantBuffer((BYTE*)&const_buffer_, sizeof(const_buffer_)));

        return S_OK;
    }


    IFACEMETHODIMP_(ULONG) ShadowTransform::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) ShadowTransform::Release() {
        auto cr = InterlockedDecrement(&ref_count_);
        if (cr == 0) {
            delete this;
        }

        return cr;
    }

    IFACEMETHODIMP ShadowTransform::QueryInterface(REFIID riid, _Outptr_ void** ppOutput)
    {
        if (ppOutput == nullptr) {
            return E_INVALIDARG;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppOutput = reinterpret_cast<IUnknown*>(this);
        } else if (IsEqualIID(riid, __uuidof(ID2D1Transform))) {
            *ppOutput = static_cast<ID2D1Transform*>(this);
        } else if (IsEqualIID(riid, __uuidof(ID2D1DrawTransform))) {
            *ppOutput = static_cast<ID2D1DrawTransform*>(this);
        } else {
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}