#include "shadow_effect.h"

#include <fstream>
#include <sstream>

#include "ukive/log.h"
#include "shadow_transform.h"


namespace shell {

    ShadowEffect::ShadowEffect()
        :ref_count_(1),
        transform_(nullptr) {}


    IFACEMETHODIMP ShadowEffect::Initialize(
        _In_ ID2D1EffectContext* pContextInternal,
        _In_ ID2D1TransformGraph* pTransformGraph) {

        std::ifstream reader("shadow_pixel_shader.cso", std::ios::binary);
        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        size_t charSize = (size_t)reader.tellg();
        reader.seekg(cpos);

        char *shaderBuf = new char[charSize];
        reader.read(shaderBuf, charSize);

        HRESULT hr = pContextInternal->LoadPixelShader(
            GUID_ShadowPixelShader, (BYTE*)shaderBuf, charSize);

        delete[] shaderBuf;

        if (FAILED(hr)) {
            return hr;
        }

        transform_ = new ShadowTransform();
        // Connects the effect's input to the transform's input, and connects
        // the transform's output to the effect's output.
        hr = pTransformGraph->SetSingleTransformNode(transform_);

        return hr;
    }

    IFACEMETHODIMP ShadowEffect::PrepareForRender(D2D1_CHANGE_TYPE changeType) {
        // All effect properties are DPI independent (specified in DIPs). In this offset
        // example, the offset value provided must be scaled from DIPs to pixels to ensure
        // a consistent appearance at different DPIs (excluding minor scaling artifacts).
        // A context's DPI can be retrieved using the ID2D1EffectContext::GetDPI API.

        //D2D1_POINT_2L pixelOffset;
        //pixelOffset.x = static_cast<LONG>(m_offset.x * (m_dpiX / 96.0f));
        //pixelOffset.y = static_cast<LONG>(m_offset.y * (m_dpiY / 96.0f));

        // Update the effect's offset transform with the new offset value.
        //m_pOffsetTransform->SetOffset(pixelOffset);

        return S_OK;
    }

    IFACEMETHODIMP ShadowEffect::SetGraph(_In_ ID2D1TransformGraph* pGraph) {
        return E_NOTIMPL;
    }

    HRESULT __stdcall ShadowEffect::Register(_In_ ID2D1Factory1* pFactory) {
        std::wifstream reader("shadow_props.xml");
        reader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf16<wchar_t>));
        DCHECK(!reader.fail());

        std::wstringstream buffer;
        buffer << reader.rdbuf();
        std::wstring contents(buffer.str());

        const D2D1_PROPERTY_BINDING bindings[] = {
            D2D1_VALUE_TYPE_BINDING(
                L"Alpha",      // The name of property. Must match name attribute in XML.
                &SetAlpha,     // The setter method that is called on "SetValue".
                &GetAlpha      // The getter method that is called on "GetValue".
            ),
            D2D1_VALUE_TYPE_BINDING(L"Shape", &SetShape, &GetShape),
            D2D1_VALUE_TYPE_BINDING(L"Bounds", &SetBounds, &GetBounds),
            D2D1_VALUE_TYPE_BINDING(L"Offset", &SetOffset, &GetOffset),
            D2D1_VALUE_TYPE_BINDING(L"Elevation", &SetElevation, &GetElevation)
        };

        HRESULT hr = pFactory->RegisterEffectFromString(
            CLSID_ShadowEffect,  // GUID defined in class header file.
            contents.c_str(),    // UCS-2BE BOM 编码。不接受 UTF-8 编码。
            bindings,            // The previously-defined property bindings array.
            ARRAYSIZE(bindings), // Number of entries in the property bindings array.
            CreateEffect         // Static method that returns an instance of the effect's class.
        );

        DCHECK(SUCCEEDED(hr));
        return hr;
    }

    HRESULT __stdcall ShadowEffect::CreateEffect(_Outptr_ IUnknown** ppEffectImpl) {
        // 假设该效果初始引用计数为1。
        *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new ShadowEffect());
        if (*ppEffectImpl == nullptr) {
            return E_OUTOFMEMORY;
        }

        return S_OK;
    }

    HRESULT ShadowEffect::SetAlpha(float alpha) {
        return S_OK;
    }

    HRESULT ShadowEffect::SetShape(SHADOW_SHAPE shape) {
        return S_OK;
    }

    HRESULT ShadowEffect::SetBounds(D2D_VECTOR_4F offset) {
        return S_OK;
    }

    HRESULT ShadowEffect::SetOffset(D2D_VECTOR_2F offset) {
        return S_OK;
    }

    HRESULT ShadowEffect::SetElevation(float elevation) {
        return S_OK;
    }

    float ShadowEffect::GetAlpha() const {
        return 0.f;
    }

    SHADOW_SHAPE ShadowEffect::GetShape() const {
        return SHADOW_SHAPE::SHADOW_SHAPE_RECT;
    }

    D2D_VECTOR_4F ShadowEffect::GetBounds() const {
        return D2D1::Vector4F(0, 0, 0, 0);
    }

    D2D_VECTOR_2F ShadowEffect::GetOffset() const {
        return D2D1::Vector2F(0, 0);
    }

    float ShadowEffect::GetElevation() const {
        return 0.f;
    }

    IFACEMETHODIMP_(ULONG) ShadowEffect::AddRef() {
        return InterlockedIncrement(&ref_count_);
    }

    IFACEMETHODIMP_(ULONG) ShadowEffect::Release() {
        auto cr = InterlockedDecrement(&ref_count_);
        if (cr == 0) {
            delete this;
        }

        return cr;
    }

    IFACEMETHODIMP ShadowEffect::QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput) {
        if (ppOutput == nullptr) {
            return E_INVALIDARG;
        }

        if (IsEqualIID(riid, IID_IUnknown)) {
            *ppOutput = this;
        } else if (IsEqualIID(riid, __uuidof(ID2D1EffectImpl))) {
            *ppOutput = this;
        } else {
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

}