#ifndef UKIVE_VIEWS_DIRECT3D_VIEW_H_
#define UKIVE_VIEWS_DIRECT3D_VIEW_H_

#include "ukive/graphics/graphic_device_manager.h"
#include "ukive/graphics/swapchain_resize_notifier.h"
#include "ukive/views/view.h"


namespace ukive {

    class Scene;

    class Direct3DView : public View {
    public:
        Direct3DView(Window* w, Scene* scene);
        ~Direct3DView();

    protected:
        void onMeasure(int width, int height, int width_spec, int height_spec) override;
        void onSizeChanged(int width, int height, int old_width, int old_height) override;
        void onDraw(Canvas* canvas) override;
        bool onInputEvent(InputEvent* e) override;

    private:
        void createStates();
        void releaseStates();

        void setViewports(float x, float y, float width, float height);

        void createResourceViews(int width, int height);
        void releaseResourceViews();

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