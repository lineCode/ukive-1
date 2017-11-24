#ifndef UKIVE_VIEWS_DIRECT3D_VIEW_H_
#define UKIVE_VIEWS_DIRECT3D_VIEW_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/swapchain_resize_notifier.h"
#include "ukive/graphics/direct3d_render_listener.h"
#include "ukive/views/view.h"


namespace ukive {

    class Direct3DView :
        public View,
        public SwapChainResizeNotifier,
        public Direct3DRenderListener
    {
    private:
        D3D11_VIEWPORT mViewport;

        ComPtr<ID3D11Texture2D> mNormalDepthStencilBuffer;
        ComPtr<ID3D11RenderTargetView> mNormalRenderTargetView;
        ComPtr<ID3D11DepthStencilView> mNormalDepthStencilView;
        ComPtr<ID3D11ShaderResourceView> mShaderResourceView;

        ComPtr<ID3D11DepthStencilState> mNormalDepthStencilState;
        ComPtr<ID3D11RasterizerState> mNormalRasterizerState;
        ComPtr<ID3D11SamplerState> mNormalSamplerState;

        void initDirect3DView();

        HRESULT createState();
        void releaseState();

        void setViewports(
            float x, float y,
            float width, float height);

        HRESULT createResourceView();
        void releaseResourceView();

    public:
        Direct3DView(Window *wnd);
        Direct3DView(Window *wnd, int id);
        ~Direct3DView();

        void onDirect3DClear() override;
        void onDirect3DRender() override;

        void onSwapChainResize() override;
        void onSwapChainResized() override;

    protected:
        void onMeasure(
            int width, int height,
            int widthSpec, int heightSpec) override;
        void onDraw(Canvas *canvas) override;
        bool onInputEvent(InputEvent *e) override;

        void onLayout(
            bool changed, bool sizeChanged,
            int left, int top, int right, int bottom) override;
        void onSizeChanged(
            int width, int height, int oldWidth, int oldHeight) override;
    };

}

#endif  // UKIVE_VIEWS_DIRECT3D_VIEW_H_