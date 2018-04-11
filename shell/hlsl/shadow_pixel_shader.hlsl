#define RADIUS    128
#define DIAMETER  (RADIUS * 2 + 1)


Texture1D stencil : register(t0);

cbuffer constants : register(b0) {
    float4 bounds_ : packoffset(c0);
    float2 offset_ : packoffset(c1);
    float alpha_ : packoffset(c1.z);
    float elevation_ : packoffset(c1.w);
    float corner_radius_ : packoffset(c2);
};

bool isInRect(float x, float y) {
    return (x >= bounds_.x && x <= bounds_.z
        && y >= bounds_.y && y <= bounds_.w);
}

bool isInRoundRect(float x, float y) {
    float4 effective = bounds_;

    if (x >= effective.x && x <= effective.z
        && y >= effective.y && y <= effective.w) {

        float dx = x - (effective.x + corner_radius_);
        float dy = y - (effective.y + corner_radius_);
        if (dx < 0 && dy < 0) {
            if (pow(dx, 2) + pow(dy, 2) < pow(corner_radius_ - 1, 2)) {
                return true;
            } else {
                return false;
            }
        }

        dx = x - (effective.z - corner_radius_);
        dy = y - (effective.y + corner_radius_);
        if (dx > 0 && dy < 0) {
            if (pow(dx, 2) + pow(dy, 2) < pow(corner_radius_ - 1, 2)) {
                return true;
            } else {
                return false;
            }
        }

        dx = x - (effective.x + corner_radius_);
        dy = y - (effective.w - corner_radius_);
        if (dx < 0 && dy > 0) {
            if (pow(dx, 2) + pow(dy, 2) < pow(corner_radius_ - 1, 2)) {
                return true;
            } else {
                return false;
            }
        }

        dx = x - (effective.z - corner_radius_);
        dy = y - (effective.w - corner_radius_);
        if (dx > 0 && dy > 0) {
            if (pow(dx, 2) + pow(dy, 2) < pow(corner_radius_ - 1, 2)) {
                return true;
            } else {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool isInOval(float x, float y) {
    float rx = (bounds_.z - bounds_.x) / 2;
    float ry = (bounds_.w - bounds_.y) / 2;
    float cx = (bounds_.z + bounds_.x) / 2;
    float cy = (bounds_.w + bounds_.y) / 2;

    return pow(x - cx, 2) / pow(rx, 2) + pow(y - cy, 2) / pow(ry, 2) < 1;
}

bool isInOutline(float x, float y) {
    return isInRect(x, y);
}

float4 getColor(float x, float y) {
    if (isInOutline(x, y)) {
        return float4(0, 0, 0, alpha_);
    } else {
        return float4(0, 0, 0, 0);
    }
}


// ��λΪ����
float4 main(
    float4 clipSpaceOutput  : SV_POSITION,     // Direct2D ��ʹ�á�
    float4 sceneSpaceOutput : SCENE_POSITION,  // ��ǰ�������ꡣ
    float4 texelSpaceInput0 : TEXCOORD0) : SV_Target
{
    float x = sceneSpaceOutput.x;
    float y = sceneSpaceOutput.y;
    if (isInOutline(x + offset_.x, y + offset_.y)) {
        discard;
        return float4(0, 0, 0, 0);
    }

    float tmp_alpha = 0;
    for (int i = 0; i < DIAMETER; ++i) {
        float4 color = getColor(i - RADIUS + x, y);

        int index_x = i;
        if (i > RADIUS) {
            index_x = 2 * RADIUS - i;
        }

        float weight = stencil.Load(int2(index_x, 0)).x;
        tmp_alpha += color.w * weight;
    }

    float total_alpha = 0;
    for (int j = 0; j < DIAMETER; ++j) {
        int index_y = j;
        if (j > RADIUS) {
            index_y = 2 * RADIUS - j;
        }

        float weight = stencil.Load(int2(index_y, 0)).x;
        total_alpha += tmp_alpha * weight;
    }

    return float4(0, 0, 0, total_alpha); // r g b a
}