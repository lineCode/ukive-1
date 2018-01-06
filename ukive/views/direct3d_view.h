#ifndef UKIVE_VIEWS_DIRECT3D_VIEW_H_
#define UKIVE_VIEWS_DIRECT3D_VIEW_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/swapchain_resize_notifier.h"
#include "ukive/views/view.h"


namespace ukive {

    class Scene;

    class Direct3DView :
        public View,
        public SwapChainResizeNotifier
    {
    public:
        Direct3DView(Window* w, Scene* scene);
        ~Direct3DView();

        void onPreSwapChainResize() override;
        void onPostSwapChainResize() override;

    protected:
        void onMeasure(int width, int height, int widthSpec, int heightSpec) override;
        void onLayout(
            bool changed, bool size_changed,
            int left, int top, int right, int bottom) override;
        void onSizeChanged(int width, int height, int oldWidth, int oldHeight) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

    private:
        void initDirect3DView();

        HRESULT createState();
        void releaseState();

        void setViewports(
            float x, float y, float width, float height);

        HRESULT createResourceView(unsigned int width, unsigned int height);
        void releaseResourceView();

        Scene* scene_;

        D3D11_VIEWPORT viewport_;

        ComPtr<ID2D1Bitmap> d3d_content_;

        ComPtr<ID3D11Texture2D> d3d_content_surface_;
        ComPtr<ID3D11Texture2D> depth_stencil_buffer_;

        ComPtr<ID3D11RenderTargetView> render_target_view_;
        ComPtr<ID3D11DepthStencilView> depth_stencil_view_;
        ComPtr<ID3D11ShaderResourceView> shader_resource_view_;

        ComPtr<ID3D11DepthStencilState> depth_stencil_state_;
        ComPtr<ID3D11RasterizerState> rasterizer_state_;
        ComPtr<ID3D11SamplerState> sampler_state_;
    };

}

#endif  // UKIVE_VIEWS_DIRECT3D_VIEW_H_