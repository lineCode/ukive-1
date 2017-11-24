
cbuffer constants : register(b0)
{
	float4 bound : packoffset(c0);
};

float4 main(
	float4 clipSpaceOutput  : SV_POSITION,     //Direct2D 不使用。
	float4 sceneSpaceOutput : SCENE_POSITION,  //当前像素坐标。
	float4 texelSpaceInput0 : TEXCOORD0
) : SV_Target
{
	float sceneWidth = bound.z - bound.x;
	float sceneHeight = bound.w - bound.y;
	float cx = (bound.z + bound.x) / 2;
	float cy = (bound.y + bound.w) / 2;
	float radius = min(sceneWidth, sceneHeight) / 2;

	float x = sceneSpaceOutput.x - cx;
	float y = sceneSpaceOutput.y - cy;

	if ((x*x + y*y) <= radius*radius)
		return float4(1, 1, 1, 1);
	else
		return float4(0, 0, 0, 0);
}