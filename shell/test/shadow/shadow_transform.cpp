#include "shadow_transform.h"

#include <cmath>

#include "ukive/utils/hresult_utils.h"

#define RADIUS 128


namespace shell {

    ShadowTransform::ShadowTransform()
        :ref_count_(1),
        expanded_(0),
        input_rect_({ 0 }),
        const_buffer_({ 0 }),
        texture_extent_(0) {
    }

    ShadowTransform::~ShadowTransform() {
    }


    HRESULT ShadowTransform::SetAlpha(float alpha) {
        const_buffer_.alpha = alpha;

        RH(draw_info_->SetPixelShaderConstantBuffer(
            reinterpret_cast<BYTE*>(&const_buffer_), sizeof(const_buffer_)));
        return S_OK;
    }

    HRESULT ShadowTransform::SetShape(SHADOW_SHAPE shape) {
        return S_OK;
    }

    HRESULT ShadowTransform::SetBounds(D2D_VECTOR_4F bounds) {
        input_rect_.left = bounds.x;
        input_rect_.top = bounds.y;
        input_rect_.right = bounds.z;
        input_rect_.bottom = bounds.w;

        const_buffer_.bounds[0] = bounds.x;
        const_buffer_.bounds[1] = bounds.y;
        const_buffer_.bounds[2] = bounds.z;
        const_buffer_.bounds[3] = bounds.w;

        RH(draw_info_->SetPixelShaderConstantBuffer(
            reinterpret_cast<BYTE*>(&const_buffer_), sizeof(const_buffer_)));
        return S_OK;
    }

    HRESULT ShadowTransform::SetOffset(D2D_VECTOR_2F offset) {
        const_buffer_.offset[0] = offset.x;
        const_buffer_.offset[1] = offset.y;

        RH(draw_info_->SetPixelShaderConstantBuffer(
            reinterpret_cast<BYTE*>(&const_buffer_), sizeof(const_buffer_)));
        return S_OK;
    }

    HRESULT ShadowTransform::SetElevation(float elevation) {
        const_buffer_.elevation = elevation;

        RH(draw_info_->SetPixelShaderConstantBuffer(
            reinterpret_cast<BYTE*>(&const_buffer_), sizeof(const_buffer_)));
        return S_OK;
    }

    HRESULT ShadowTransform::SetCornerRadius(float radius) {
        const_buffer_.corner_radius = radius;

        RH(draw_info_->SetPixelShaderConstantBuffer(
            reinterpret_cast<BYTE*>(&const_buffer_), sizeof(const_buffer_)));
        return S_OK;
    }

    HRESULT ShadowTransform::SetTexture(
        ID2D1ResourceTexture* texture, UINT32 extent) {
        if (texture_extent_ != extent) {
            RH(draw_info_->SetResourceTexture(0, texture));
            texture_extent_ = extent;
        }

        return S_OK;
    }


    IFACEMETHODIMP_(UINT32) ShadowTransform::GetInputCount() const {
        return 0;
    }

    IFACEMETHODIMP ShadowTransform::MapInputRectsToOutputRect(
        const D2D1_RECT_L* pInputRects,
        const D2D1_RECT_L* pInputOpaqueSubRects,
        UINT32 inputRectCount,
        D2D1_RECT_L* pOutputRect,
        D2D1_RECT_L* pOutputOpaqueSubRect)
    {
        // 该变换没有输入。
        if (inputRectCount != 0) {
            return E_INVALIDARG;
        }

        (*pOutputRect).left = input_rect_.left - RADIUS;
        (*pOutputRect).top = input_rect_.top - RADIUS;
        (*pOutputRect).right = input_rect_.right + RADIUS;
        (*pOutputRect).bottom = input_rect_.bottom + RADIUS;

        // Indicate that the image's opacity has not changed.
        *pOutputOpaqueSubRect = pInputOpaqueSubRects[0];

        return S_OK;
    }

    IFACEMETHODIMP ShadowTransform::MapOutputRectToInputRects(
        const D2D1_RECT_L* pOutputRect,
        D2D1_RECT_L* pInputRects,
        UINT32 inputRectCount) const
    {
        // 该变换没有输入
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
        D2D1_RECT_L* pInvalidOutputRect) const
    {
        // If part of the transform's input is invalid, mark the corresponding
        // output region as invalid.
        *pInvalidOutputRect = invalidInputRect;

        return S_OK;
    }

    IFACEMETHODIMP ShadowTransform::SetDrawInfo(
        ID2D1DrawInfo* drawInfo)
    {
        draw_info_ = drawInfo;

        RH(drawInfo->SetPixelShader(GUID_ShadowPixelShader));
        RH(drawInfo->SetPixelShaderConstantBuffer(
            reinterpret_cast<BYTE*>(&const_buffer_), sizeof(const_buffer_)));

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