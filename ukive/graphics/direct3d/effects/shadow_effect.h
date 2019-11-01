#ifndef UKIVE_GRAPHICS_DIRECT3D_EFFECTS_SHADOW_EFFECT_H_
#define UKIVE_GRAPHICS_DIRECT3D_EFFECTS_SHADOW_EFFECT_H_

#include "ukive/graphics/graphic_device_manager.h"

#include "shell/third_party/directx_math/Inc/DirectXMath.h"


namespace ukive {

    namespace dx = DirectX;

    class Canvas;

    class ShadowEffect {
    public:
        ShadowEffect();

        void draw();
        void draw(Canvas* c);
        void setRadius(int radius);
        void setContent(ID3D11Texture2D* texture);

        int getRadius() const;
        ComPtr<ID2D1Bitmap> getOutput(ID2D1RenderTarget* rt);

    private:
        struct ConstBuffer {
            dx::XMFLOAT4X4 wvo;
        };

        struct PSConstBuffer {
            int32_t vertical;
            int32_t colored_shadow;
            int32_t vertical3;
            int32_t vertical4;
        };

        struct VertexData {
            dx::XMFLOAT3 position;
            dx::XMFLOAT2 texcoord;

            VertexData()
                :position(dx::XMFLOAT3(0, 0, 0)),
                texcoord(dx::XMFLOAT2(0, 0)) {}

            VertexData(const dx::XMFLOAT3& pos, const dx::XMFLOAT2& tex)
                :position(pos),
                texcoord(tex) {}
        };

        void createTexture(
            ComPtr<ID3D11Texture2D>& tex,
            ComPtr<ID3D11RenderTargetView>& rtv,
            ComPtr<ID3D11ShaderResourceView>& srv);

        void createKernelTexture();

        void setSize(int width, int height);

        int width_;
        int height_;
        int view_width_;
        int view_height_;
        int radius_;
        float elevation_;

        dx::XMFLOAT4X4 wvo_matrix_;
        dx::XMFLOAT4X4 world_matrix_;
        dx::XMFLOAT4X4 view_matrix_;
        dx::XMFLOAT4X4 ortho_matrix_;

        ComPtr<ID3D11RenderTargetView> bg_rtv_;
        ComPtr<ID3D11ShaderResourceView> bg_srv_;

        ComPtr<ID3D11Texture2D> kernel_tex2d_;
        ComPtr<ID3D11ShaderResourceView> kernel_srv_;

        ComPtr<ID3D11Texture2D> shadow1_tex2d_;
        ComPtr<ID3D11RenderTargetView> shadow1_rtv_;
        ComPtr<ID3D11ShaderResourceView> shadow1_srv_;

        ComPtr<ID3D11Texture2D> shadow2_tex2d_;
        ComPtr<ID3D11RenderTargetView> shadow2_rtv_;
        ComPtr<ID3D11ShaderResourceView> shadow2_srv_;

        ComPtr<ID3D11Buffer> vert_buffer_;
        ComPtr<ID3D11Buffer> index_buffer_;
        ComPtr<ID3D11Buffer> const_buffer_;
        ComPtr<ID3D11Buffer> ps_const_buffer_;
        ComPtr<ID3D11InputLayout> input_layout_;
        ComPtr<ID3D11PixelShader> pixel_shader_;
        ComPtr<ID3D11VertexShader> vertex_shader_;

        D3D11_VIEWPORT viewport_;
        ComPtr<ID3D11RasterizerState> rasterizer_state_;
    };

}

#endif  // UKIVE_GRAPHICS_DIRECT3D_EFFECTS_SHADOW_EFFECT_H_