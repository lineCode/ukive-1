#define SIGMA     3
#define RADIUS    9
#define DIAMETER  19

#define OFFSET_X  0
#define OFFSET_Y  0

#define ROUND_RECT_RADIUS  8


cbuffer constants : register(b0)
{
    float4 bound : packoffset(c0);
};

bool isInRect(float2 p) {
    float4 effective = bound;
    effective.xy += RADIUS;
    effective.zw -= RADIUS;

    return (p.x >= effective.x && p.x <= effective.z
        && p.y >= effective.y && p.y <= effective.w);
}

bool isInRoundRect(float x, float y) {
    float4 effective = bound;
    effective.xy += RADIUS;
    effective.zw -= RADIUS;

    if (x >= effective.x && x <= effective.z
        && y >= effective.y && y <= effective.w) {

        float dx = x - (effective.x + ROUND_RECT_RADIUS);
        float dy = y - (effective.y + ROUND_RECT_RADIUS);
        if (dx < 0 && dy < 0) {
            if (pow(dx, 2) + pow(dy, 2) <= pow(ROUND_RECT_RADIUS, 2)) {
                return true;
            } else {
                return false;
            }
        }

        dx = x - (effective.z - ROUND_RECT_RADIUS);
        dy = y - (effective.y + ROUND_RECT_RADIUS);
        if (dx > 0 && dy < 0) {
            if (pow(dx, 2) + pow(dy, 2) <= pow(ROUND_RECT_RADIUS, 2)) {
                return true;
            } else {
                return false;
            }
        }

        dx = x - (effective.x + ROUND_RECT_RADIUS);
        dy = y - (effective.w - ROUND_RECT_RADIUS);
        if (dx < 0 && dy > 0) {
            if (pow(dx, 2) + pow(dy, 2) <= pow(ROUND_RECT_RADIUS, 2)) {
                return true;
            } else {
                return false;
            }
        }

        dx = x - (effective.z - ROUND_RECT_RADIUS);
        dy = y - (effective.w - ROUND_RECT_RADIUS);
        if (dx > 0 && dy > 0) {
            if (pow(dx, 2) + pow(dy, 2) <= pow(ROUND_RECT_RADIUS, 2)) {
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
    float4 effective = bound;
    effective.xy += RADIUS;
    effective.zw -= RADIUS;

    float rx = (effective.z - effective.x) / 2;
    float ry = (effective.w - effective.y) / 2;
    float cx = (effective.z + effective.x) / 2;
    float cy = (effective.w + effective.y) / 2;

    return pow(x - cx, 2) / pow(rx, 2) + pow(y - cy, 2) / pow(ry, 2) <= 1;
}

bool isInOutline(float x, float y) {
    return isInRoundRect(x, y);
}

float getWeight(float x, float y) {
    float exponent = -(pow(x, 2) + pow(y, 2)) / (2 * pow(SIGMA, 2));
    return 1 / (2 * 3.14f * pow(SIGMA, 2)) * exp(exponent);
}

float4 getColor(float x, float y) {
    if (isInOutline(x, y)) {
        return float4(0, 0, 0, 0.4);
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
    float sceneWidth = bound.z - bound.x;
    float sceneHeight = bound.w - bound.y;

    float x = sceneSpaceOutput.x;
    float y = sceneSpaceOutput.y;

    float total_weight = 0;
    float weight_matrix[DIAMETER][DIAMETER];
    for (int i = 0; i < DIAMETER; ++i) {
        for (int j = 0; j < DIAMETER; ++j) {
            float w = getWeight(abs(j - RADIUS), abs(i - RADIUS));
            weight_matrix[i][j] = w;
            total_weight += w;
        }
    }

    for (i = 0; i < DIAMETER; ++i) {
        for (int j = 0; j < DIAMETER; ++j) {
            weight_matrix[i][j] /= total_weight;
        }
    }

    float total_alpha = 0;
    for (i = 0; i < DIAMETER; ++i) {
        for (int j = 0; j < DIAMETER; ++j) {
            float4 color = getColor(j - RADIUS + x, i - RADIUS + y);
            total_alpha += color.w * weight_matrix[i][j];
        }
    }

    if (isInOutline(x + OFFSET_X, y + OFFSET_Y)) {
        return float4(0, 0, 0, 0);
    } else {
        return float4(0, 0, 0, total_alpha);  // r g b a
    }
}