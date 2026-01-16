

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
cbuffer ConstantBuffer : register(b0)
{
    matrix worldViewProjection;
    float2 uvOffset;
    float2 uvRange;
}

// マテリアルバッファ
cbuffer MaterialBuffer : register(b1)
{
    float4 color;
}

cbuffer LightBuffer : register(b2)
{
    float4 ambientLight;
    float4 driectLight;
    float4 driectLightLocal;
}

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

//=============================================================================
// 頂点シェーダ
//=============================================================================
void VertexShaderPolygon(in float4 inPosition : POSITION0,
						  in float4 inNormal : NORMAL0,
						  in float4 inDiffuse : COLOR0,
						  in float2 inTexCoord : TEXCOORD0,

						  out float4 outPosition : SV_POSITION,
						  out float4 outNormal : NORMAL0,
						  out float2 outTexCoord : TEXCOORD0,
						  out float4 outDiffuse : COLOR0)
{
    outPosition = mul(inPosition, worldViewProjection);
    
    outNormal = inNormal;
    outTexCoord = inTexCoord * uvRange + uvOffset;

    outDiffuse = inDiffuse * color;
    
    
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
void PixelShaderTexture(in float4 inPosition : POSITION0,
						 in float4 inNormal : NORMAL0,
						 in float2 inTexCoord : TEXCOORD0,
						 in float4 inDiffuse : COLOR0,

						 out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord) * inDiffuse;
    
    float3 light = ambientLight.rgb * ambientLight.a; // float3(0.0f, 0.0f, 0.0f);
    light += driectLight.rgb * driectLight.a * saturate(dot(normalize(inNormal.xyz), normalize(driectLightLocal.xyz)));
    //outDiffuse.rgb *= ambientLight.rgb * ambientLight.a;
    outDiffuse.rgb *= light;
    
    
    // 視程フェードアウト
    //float far = 2000.0f; // 視程
    //float border = 0.8f; // 境目、パーセント
    //float d = distance(inPosition.xyz, float3(0.0f, 0.0f, 0.0f));
    //outDiffuse.a *= 1.0f - saturate(d / far - border) / (1.0f - border);
}

//=============================================================================
// ピクセルシェーダ、テクスチャなし
//=============================================================================
void PixelShaderPolygon(in float4 inPosition : POSITION0,
						 in float4 inNormal : NORMAL0,
						 in float2 inTexCoord : TEXCOORD0,
						 in float4 inDiffuse : COLOR0,

						 out float4 outDiffuse : SV_Target)
{
    outDiffuse = inDiffuse;
    float3 light = ambientLight.rgb * ambientLight.a; // float3(0.0f, 0.0f, 0.0f);
    light += driectLight.rgb * driectLight.a * saturate(dot(normalize(inNormal.xyz), normalize(driectLightLocal.xyz)));
    //outDiffuse.rgb *= ambientLight.rgb * ambientLight.a;
    outDiffuse.rgb *= light;
}

//=============================================================================
// ピクセルシェーダ、ライティングなし
//=============================================================================
void PixelShaderNoLighting(in float4 inPosition : POSITION0,
						 in float4 inNormal : NORMAL0,
						 in float2 inTexCoord : TEXCOORD0,
						 in float4 inDiffuse : COLOR0,

						 out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord) * inDiffuse;
    
}


