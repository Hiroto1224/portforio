struct CAMERA
{
	float4 position;
	float4 direction;
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 viewProjection;
	row_major float4x4 inverseView;
	row_major float4x4 inverseProjection;
	row_major float4x4 inverseViewProjection;
};

struct DIRECTIONAL_LIGHT
{
	float4 direction;
	float4 color; //w:directional_light_intensity
};
struct POINT_LIGHT
{
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float dummy;
	float4 pos;
	float4 color;
};
struct SPOT_LIGHT
{
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float innerCorn;
	float outerCorn;
	float dummy0;
	float dummy1;
	float dummy2;
	float4 pos;
	float4 color;
	float4 dir;
};
cbuffer CAMERA_CONSTANTS : register(b1)
{
	CAMERA camera;
};

#define POINTMAX 32
#define SPOTMAX 32

cbuffer LIGHT_CONSTATNS : register(b2)
{
	float4 ambient;
	POINT_LIGHT pointLight[POINTMAX];
	SPOT_LIGHT spotLight[SPOTMAX];
	DIRECTIONAL_LIGHT directionalLight;
};

