cbuffer cbData {
    int vertical;
    int vertical2;
    int vertical3;
    int vertical4;
};

Texture2D shader_texture_ : register(t0);
Texture2D<float> kernel_texture_ : register(t1);

SamplerState sampler_state_;


struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
    float2 tex : TEXCOORD0;
};


float4 main(PixelInputType input) : SV_TARGET {
    uint width = 0, height = 0;
    kernel_texture_.GetDimensions(width, height);

    float tmp_alpha = 0;
    int radius = width - 1;
    int diameter = width * 2 - 1;
    for (int i = 0; i < diameter; ++i) {
        float x = 0, y = 0;
        if (vertical == 0) {
            x = i - radius + input.raw_position.x;
            y = input.raw_position.y;
        } else {
            x = input.raw_position.x;
            y = i - radius + input.raw_position.y;
        }

        float4 color = shader_texture_.Load(int3(x, y, 0));

        int index_x = i;
        if (i > radius) {
            index_x = 2 * radius - i;
        }

        float weight = kernel_texture_.Load(int3(index_x, 0, 0));
        tmp_alpha += color.w * 0.5f * weight;
    }

    return float4(0, 0, 0, tmp_alpha);
}