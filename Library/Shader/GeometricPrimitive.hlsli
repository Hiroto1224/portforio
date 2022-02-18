

//struct VS_OUT
//{
//	float4 position : SV_POSITION;
//	float4 colour : COLOUR;
//};
//
//cbuffer CONSTANT_BUFFER : register(b0)
//{
//	row_major float4x4 WVP;
//	row_major float4x4 world;
//	float4 material_Colour;
//	float4 light_Dir;
//
//}

//--------------------------------------------
//	テクスチャ
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

Texture2D ShadowTexture : register(t3);
SamplerState ShadowSampler : register(s3);

//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	matrix  World;
	matrix	matWVP;
};

cbuffer CBPerFrame2 : register(b2)
{
	float4	LightColor;		//ライトの色
	float4	LightDir;		//ライトの方向
	float4	AmbientColor;	//環境光
	float4  EyePos;			//カメラ座標
};
cbuffer CBPerFrame3 : register(b3)
{
	float4x4 LightViewProjection;
	float4	ShadowColor;

};


static const float Bias = 0.0008;
//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Colour    : COLOUR;	//頂点カラー無し
};



//バーテックスシェーダー出力構造体
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Colour : COLOUR0;
	float3 wNormal : TEXCOORD1;	//ワールド法線
	float3 wPosition : TEXCOORD2;//ワールド座標
	float2 Tex : TEXCOORD3;
	float3 vShadow : TEXCOORD4;
};

//ワールド座標をライト空間座標に変換
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
	// 正規化デバイス座標系
	float4 wvpPos;
	wvpPos.xyz = wPos;
	wvpPos.w = 1;
	wvpPos = mul(vp, wvpPos);
	wvpPos /= wvpPos.w;
	// テクスチャ座標系
	wvpPos.y = -wvpPos.y;
	wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
	return wvpPos.xyz;
}

//シャドーマップからシャドー空間座標に変換とZ値比較
float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor,
	float Bias)
{ // シャドウマップから深度を取り出す
	float d = st.Sample(ss, Tex.xy).r;
	// シャドウマップの深度値と現実の深度の比較
	Scolor = (Tex.z - d > Bias) ? Scolor : float3(1, 1, 1);
	return Scolor;
}

//--------------------------------------------
//	拡散反射関数(ランバート)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率

float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// 負の値を０にする
	return C * D * K;
}

//--------------------------------------------
//	ハーフランバート
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 HalfLambert(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = D * 0.5 + 0.5;
	//D = D * D;	// いい感じに減衰する
	return C * D * K;
}





//--------------------------------------------
//	鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//ハーフベクトル
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}
//--------------------------------------------
//	鏡面反射関数(フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//反射ベクトル
	float3 R = normalize(reflect(L, N));
	float3 S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}