cbuffer cbData {
    int vertical;
    int colored_shadow;
    int unused1;
    int unused2;
};

Texture2D shader_texture_ : register(t0);
Texture2D<float> kernel_texture_ : register(t1);


struct PixelInputType {
    float4 position : SV_POSITION;
    float3 raw_position : POSITION;
};


float4 main(PixelInputType input) : SV_TARGET {
    int width = 0, height = 0;
    kernel_texture_.GetDimensions(width, height);

    float acc_alpha = 0;
    float4 acc_color = 0;
    int radius = width - 1;
    int diameter = width * 2 - 1;
    for (int i = 0; i < diameter; ++i) {
        int x = 0, y = 0;
        if (vertical == 0) {
            x = i - radius + (int)floor(input.raw_position.x) - radius;
            y = (int)floor(input.raw_position.y) - radius;
        } else {
            x = (int)floor(input.raw_position.x);
            y = i - radius + (int)floor(input.raw_position.y);
        }

        float4 color = shader_texture_.Load(int3(x, y, 0));

        int index_x = i;
        if (i > radius) {
            index_x = 2 * radius - i;
        }

        float weight = kernel_texture_.Load(int3(index_x, 0, 0));
        acc_alpha += color.w * weight;
        //acc_color += color * weight;
    }

    //return acc_color;
    return float4(0, 0, 0, acc_alpha * 0.6f);
}