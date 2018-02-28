#define RADIUS    12
#define DIAMETER  (RADIUS * 2 + 1)


Texture2D stencil : register(t0);

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
    return isInRoundRect(x, y);
}

float4 getColor(float x, float y) {
    if (isInOutline(x, y)) {
        return float4(0, 0, 0, alpha_);
    } else {
        return float4(0, 0, 0, 0);
    }
}


// 单位为像素
float4 main(
    float4 clipSpaceOutput  : SV_POSITION,     // Direct2D 不使用。
    float4 sceneSpaceOutput : SCENE_POSITION,  // 当前像素坐标。
    float4 texelSpaceInput0 : TEXCOORD0) : SV_Target
{
    float x = sceneSpaceOutput.x;
    float y = sceneSpaceOutput.y;
    if (isInOutline(x + offset_.x, y + offset_.y)) {
        discard;
        return float4(0, 0, 0, 0);
    }

    float total_alpha = 0;
    for (int i = 0; i < DIAMETER; ++i) {
        for (int j = 0; j < DIAMETER; ++j) {
            float4 color = getColor(j - RADIUS + x, i - RADIUS + y);

            int index_x = j;
            int index_y = i;
            if (i > RADIUS) {
                index_y = 2 * RADIUS - i;
            }
            if (j > RADIUS) {
                index_x = 2 * RADIUS - j;
            }

            if (index_x < index_y) {
                int tmp = index_x;
                index_x = index_y;
                index_y = tmp;
            }

            float weight = stencil.Load(int3(index_x, index_y, 0)).x;
            total_alpha += color.w * weight;
        }
    }

    return float4(0, 0, 0, total_alpha); // r g b a
}