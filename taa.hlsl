cbuffer cbConstants : register( b0 )
{
    float4x4 g_f4x4WorldViewProjection;        // World * View * Projection matrix
	float4   g_fParams;                        // screen_width, screen_height, blending weight, blur_size
}

// Textures
Texture2D         g_txAcc      : register(t0);
Texture2D         g_txScene     : register(t1);

// Samplers
SamplerState                g_SampleLinear      : register(s0);

struct VS_RenderSceneInput
{
    float3 Position    : POSITION;  
	float2 TexCoord    : TEXTURE0;
};

struct PS_RenderSceneInput
{
    float4 Position   : SV_Position;
	float2 TexCoord   : TEXTURE0;
};

struct PS_RenderOutput
{
    float4 Color    : SV_Target0;
};

PS_RenderSceneInput VS( VS_RenderSceneInput In )
{
    PS_RenderSceneInput O;
    
	O.Position = mul(float4(In.Position, 1.0f), g_f4x4WorldViewProjection);
    O.TexCoord = In.TexCoord;

    return O;    
}

float3 RGB2YCbCr(float3 rgb)
{
	float3 RGB2Y = { 0.29900, 0.58700, 0.11400 };
	float3 RGB2Cb = {-0.16874, -0.33126, 0.50000 };
	float3 RGB2Cr = { 0.50000, -0.41869, -0.081 };

	return float3(dot(rgb, RGB2Y), dot(rgb, RGB2Cb), dot(rgb, RGB2Cr));
}

float3 YCbCr2RGB(float3 ycc)
{
	float3 YCbCr2R = { 1.0, 0.00000, 1.40200 };
	float3 YCbCr2G = { 1.0,-0.34414, -0.71414 };
	float3 YCbCr2B = { 1.0, 1.77200, 1.40200 };

	return float3(dot(ycc, YCbCr2R), dot(ycc, YCbCr2G), dot(ycc, YCbCr2B));
}



PS_RenderOutput PS( PS_RenderSceneInput In )
{
    PS_RenderOutput O;

	O.Color = 0;

	float2 neighbor_offset[4] = {
			{ 0, +1 },
			{ 0, -1 },
			{+1,  0 },
			{-1,  0 },
	};

	float4 center_color = g_txScene.Sample(g_SampleLinear, In.TexCoord);

	float4 neighbor_sum = center_color;

	for (int i = 0; i < 4; i++){
		// 近傍の点を取る
		float4 neighbor = g_txAcc.Sample(g_SampleLinear, In.TexCoord + neighbor_offset[i] * g_fParams.xy * g_fParams.w);
		float3 color_diff = abs(neighbor.xyz - center_color.xyz);
		float3 ycc = RGB2YCbCr(color_diff.xyz);		// 中心との差をYCbCrで見る
		const float cbcr_threshhold = 0.32f;
		float cbcr_len = length(color_diff.yz);
		if (cbcr_threshhold < cbcr_len){// 色相成分が大きく異なる時、閾値に収まる範囲に色を補正して合成
			ycc = (cbcr_threshhold / cbcr_len) * ycc;
			neighbor.rgb = center_color.rgb + YCbCr2RGB(ycc);
		}
		neighbor_sum += neighbor;
	}
	O.Color = neighbor_sum / 5.0f;

	return O;
}
