#ifndef SHELL_TEST_ANIMATION_WALLPAPER_EFFECT_H_
#define SHELL_TEST_ANIMATION_WALLPAPER_EFFECT_H_

#include <codecvt>
#include <initguid.h>
#include <d2d1effects_1.h>
#include <d2d1effectauthor.h>
#include <d2d1effecthelpers.h>


namespace shell {

    class WallpaperTransform;

    enum WALLPAPER_EFFECT_PROP
    {
        WALLPAPER_EFFECT_PROP_OFFSET = 0
    };

    // ����Ψһ��ʶ��Ч����GUID������������Direct2Dע���Ч����
    // ���ڿ����ߵ��� ID2D1DeviceContext::CreateEffect ����ʱ
    // ��ʶ��Ч������Ч��������������Ӧ���б���Ψһ��GUIDҲһ����
    // {0371E149-212E-41AD-A12F-3AC9328C1E17}
    DEFINE_GUID(CLSID_UWallpaperEffect, 0x371e149, 0x212e, 0x41ad, 0xa1, 0x2f, 0x3a, 0xc9, 0x32, 0x8c, 0x1e, 0x17);


    class WallpaperEffect : public ID2D1EffectImpl
    {
    private:
        // ���캯������˽���Ա�֤�䲻�������á�
        WallpaperEffect();

        LONG mRefCount;
        WallpaperTransform *mTransform;

    public:
        // ��Ӧ�õ��� ID2D1DeviceContext::CreateEffect ��Direct2D
        // ����ô˷�����������ڴ˷����н����ڲ���ʼ����������Ч���йصĹ�����
        // ���⣬�㻹�����ڴ˴�����ʼ�任ͼ�Ρ�
        IFACEMETHODIMP Initialize(
            _In_ ID2D1EffectContext* pContextInternal,
            _In_ ID2D1TransformGraph* pTransformGraph
        );

        IFACEMETHODIMP PrepareForRender(D2D1_CHANGE_TYPE changeType);
        IFACEMETHODIMP SetGraph(_In_ ID2D1TransformGraph* pGraph);

        static HRESULT __stdcall Register(_In_ ID2D1Factory1* pFactory);
        static HRESULT __stdcall CreateEffect(_Outptr_ IUnknown** ppEffectImpl);

        HRESULT SetOffset(D2D_VECTOR_2F offset);
        D2D_VECTOR_2F GetOffset() const;

        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(_In_ REFIID riid, _Outptr_ void** ppOutput);
    };

}

#endif  // SHELL_TEST_ANIMATION_WALLPAPER_EFFECT_H_