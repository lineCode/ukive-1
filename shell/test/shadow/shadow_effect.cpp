#include "shadow_effect.h"

#include <codecvt>
#include <fstream>
#include <sstream>

#include "ukive/log.h"
#include "shadow_transform.h"

#define RADIUS 64
#define ELEVATION (RADIUS / 2)


namespace {
    float getWeight(float x, float y, float sigma) {
        float exponent = -(std::pow(x, 2) + std::pow(y, 2)) / (2 * std::pow(sigma, 2));
        return 1 / (2 * 3.14f * std::pow(sigma, 2)) * std::exp(exponent);
    }
}


namespace shell {

    ShadowEffect::ShadowEffect()
        :ref_count_(1),
        transform_(nullptr),
        alpha_(1.f),
        elevation_(0.f),
        corner_radius_(0.f),
        shape_(SHADOW_SHAPE_RECT),
        bounds_({ 0 }),
        offset_({ 0 }) {}


    HRESULT ShadowEffect::CreateGaussStencil(ID2D1EffectContext* ec) {
        UINT32 extent[2] = { RADIUS + 1, RADIUS + 1 };
        D2D1_EXTEND_MODE mode[2] = { D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP };

        D2D1_RESOURCE_TEXTURE_PROPERTIES props;
        props.extents = extent;
        props.dimensions = 2;
        props.bufferPrecision = D2D1_BUFFER_PRECISION_32BPC_FLOAT;
        props.channelDepth = D2D1_CHANNEL_DEPTH_1;
        props.filter = D2D1_FILTER_MIN_MAG_MIP_POINT;
        props.extendModes = mode;

        UINT32 stride = sizeof(float) * (RADIUS + 1);

        float total_weight = 0;
        float weight_matrix[RADIUS + 1][RADIUS + 1];
        for (int i = 0; i < RADIUS + 1; ++i) {
            for (int j = i; j < RADIUS + 1; ++j) {
                float w = getWeight(RADIUS - j, RADIUS - i, ELEVATION);
                weight_matrix[i][j] = w;
                if (i != RADIUS && j != RADIUS) {
                    total_weight += w;
                }
            }
        }

        total_weight *= 8;
        for (int i = 0; i < RADIUS; ++i) {
            total_weight -= weight_matrix[i][i] * 4;
            total_weight -= weight_matrix[i][RADIUS] * 4;
        }
        total_weight += weight_matrix[RADIUS][RADIUS];

        for (int i = 0; i < RADIUS + 1; ++i) {
            for (int j = i; j < RADIUS + 1; ++j) {
                weight_matrix[i][j] /= total_weight;
            }
        }

        HRESULT hr = ec->CreateResourceTexture(
            nullptr, &props, reinterpret_cast<BYTE*>(weight_matrix),
            &stride, sizeof(float) * (RADIUS + 1) * (RADIUS + 1), &texture_);
        if (FAILED(hr)) {
            DCHECK(false);
            return hr;
        }

        return S_OK;
    }

    IFACEMETHODIMP ShadowEffect::Initialize(
        ID2D1EffectContext* pContextInternal,
        ID2D1TransformGraph* pTransformGraph) {

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
            DCHECK(false);
            return hr;
        }

        CreateGaussStencil(pContextInternal);

        transform_ = new ShadowTransform();
        // Connects the effect's input to the transform's input, and connects
        // the transform's output to the effect's output.
        hr = pTransformGraph->SetSingleTransformNode(transform_.get());

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

        transform_->SetAlpha(alpha_);
        transform_->SetShape(shape_);
        transform_->SetBounds(bounds_);
        transform_->SetOffset(offset_);
        transform_->SetElevation(elevation_);
        transform_->SetCornerRadius(corner_radius_);
        transform_->SetTexture(texture_.get(), 1);

        return S_OK;
    }

    IFACEMETHODIMP ShadowEffect::SetGraph(ID2D1TransformGraph* pGraph) {
        return E_NOTIMPL;
    }

    HRESULT __stdcall ShadowEffect::Register(ID2D1Factory1* pFactory) {
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
            D2D1_VALUE_TYPE_BINDING(L"Elevation", &SetElevation, &GetElevation),
            D2D1_VALUE_TYPE_BINDING(L"CornerRadius", &SetCornerRadius, &GetCornerRadius)
        };

        HRESULT hr = pFactory->RegisterEffectFromString(
            CLSID_ShadowEffect,  // GUID defined in class header file.
            contents.c_str(),    // UCS-2BE BOM 编码。
            bindings,            // The previously-defined property bindings array.
            ARRAYSIZE(bindings), // Number of entries in the property bindings array.
            CreateEffect         // Static method that returns an instance of the effect's class.
        );

        DCHECK(SUCCEEDED(hr));
        return hr;
    }

    HRESULT __stdcall ShadowEffect::CreateEffect(IUnknown** ppEffectImpl) {
        // 假设该效果初始引用计数为1。
        *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new ShadowEffect());
        if (*ppEffectImpl == nullptr) {
            return E_OUTOFMEMORY;
        }

        return S_OK;
    }

    HRESULT ShadowEffect::SetAlpha(float alpha) {
        alpha_ = alpha;
        return S_OK;
    }

    HRESULT ShadowEffect::SetShape(SHADOW_SHAPE shape) {
        shape_ = shape;
        return S_OK;
    }

    HRESULT ShadowEffect::SetBounds(D2D_VECTOR_4F bounds) {
        bounds_ = bounds;
        return S_OK;
    }

    HRESULT ShadowEffect::SetOffset(D2D_VECTOR_2F offset) {
        offset_ = offset;
        return S_OK;
    }

    HRESULT ShadowEffect::SetElevation(float elevation) {
        elevation_ = elevation;
        return S_OK;
    }

    HRESULT ShadowEffect::SetCornerRadius(float radius) {
        corner_radius_ = radius;
        return S_OK;
    }

    float ShadowEffect::GetAlpha() const {
        return alpha_;
    }

    SHADOW_SHAPE ShadowEffect::GetShape() const {
        return shape_;
    }

    D2D_VECTOR_4F ShadowEffect::GetBounds() const {
        return bounds_;
    }

    D2D_VECTOR_2F ShadowEffect::GetOffset() const {
        return offset_;
    }

    float ShadowEffect::GetElevation() const {
        return elevation_;
    }

    float ShadowEffect::GetCornerRadius() const {
        return corner_radius_;
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

    IFACEMETHODIMP ShadowEffect::QueryInterface(REFIID riid, void** ppOutput) {
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