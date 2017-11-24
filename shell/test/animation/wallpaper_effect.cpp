#include "wallpaper_effect.h"

#include <fstream>
#include <sstream>

#include "shell/test/animation/wallpaper_transform.h"


namespace shell {

    WallpaperEffect::WallpaperEffect()
        :mRefCount(1)
    {
        mTransform = nullptr;
    }


    IFACEMETHODIMP WallpaperEffect::Initialize(
        _In_ ID2D1EffectContext* pContextInternal,
        _In_ ID2D1TransformGraph* pTransformGraph
    )
    {
        std::ifstream reader("UWallpaperPixelShader.cso", std::ios::binary);
        auto cpos = reader.tellg();
        reader.seekg(0, std::ios_base::end);
        size_t charSize = (size_t)reader.tellg();
        reader.seekg(cpos);

        char *shaderBuf = new char[charSize];
        reader.read(shaderBuf, charSize);

        HRESULT hr = pContextInternal->LoadPixelShader(
            GUID_UWallpaperPixelShader, (BYTE*)shaderBuf, charSize);

        delete[] shaderBuf;

        if (FAILED(hr))
            return hr;

        mTransform = new WallpaperTransform();
        // Connects the effect's input to the transform's input, and connects
        // the transform's output to the effect's output.
        hr = pTransformGraph->SetSingleTransformNode(mTransform);

        return hr;
    }

    IFACEMETHODIMP WallpaperEffect::PrepareForRender(D2D1_CHANGE_TYPE changeType)
    {
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

    IFACEMETHODIMP WallpaperEffect::SetGraph(_In_ ID2D1TransformGraph* pGraph)
    {
        return E_NOTIMPL;
    }

    HRESULT __stdcall WallpaperEffect::Register(_In_ ID2D1Factory1* pFactory)
    {
        std::wifstream reader("uwallpaper_props.xml");
        reader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf16<wchar_t>));

        std::wstringstream buffer;
        buffer << reader.rdbuf();
        std::wstring contents(buffer.str());

        const D2D1_PROPERTY_BINDING bindings[] =
        {
            D2D1_VALUE_TYPE_BINDING(
                L"Offset",      // The name of property. Must match name attribute in XML.
                &SetOffset,     // The setter method that is called on "SetValue".
                &GetOffset      // The getter method that is called on "GetValue".
            )
        };

        HRESULT hr = pFactory->RegisterEffectFromString(
            CLSID_UWallpaperEffect,  // GUID defined in class header file.
            contents.c_str(),              // UCS-2BE BOM 编码。不接受 UTF-8 编码。
            bindings,            // The previously-defined property bindings array.
            ARRAYSIZE(bindings), // Number of entries in the property bindings array.
            CreateEffect         // Static method that returns an instance of the effect's class.
        );

        return hr;
    }

    HRESULT __stdcall WallpaperEffect::CreateEffect(_Outptr_ IUnknown** ppEffectImpl)
    {
        // 假设该效果初始引用计数为1。
        *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new WallpaperEffect());

        if (*ppEffectImpl == nullptr)
            return E_OUTOFMEMORY;

        return S_OK;
    }


    HRESULT WallpaperEffect::SetOffset(D2D_VECTOR_2F offset)
    {
        return S_OK;
    }

    D2D_VECTOR_2F WallpaperEffect::GetOffset() const
    {
        return D2D1::Vector2F(0, 0);
    }


    IFACEMETHODIMP_(ULONG) WallpaperEffect::AddRef()
    {
        return InterlockedIncrement(&mRefCount);
    }

    IFACEMETHODIMP_(ULONG) WallpaperEffect::Release()
    {
        LONG cr = InterlockedDecrement(&mRefCount);

        if (mRefCount == 0)
            delete this;

        return cr;
    }

    IFACEMETHODIMP WallpaperEffect::QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput)
    {
        if (ppOutput == nullptr)
            return E_INVALIDARG;

        *ppOutput = nullptr;

        if (IsEqualIID(riid, IID_IUnknown))
        {
            *ppOutput = reinterpret_cast<IUnknown*>(this);
        }
        else if (IsEqualIID(riid, __uuidof(ID2D1EffectImpl)))
        {
            *ppOutput = (ID2D1EffectImpl*)this;
        }

        if (*ppOutput)
        {
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

}