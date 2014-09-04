cbuffer cbConstants : register( b0 )
{
    float4x4 g_f4x4WorldViewProjection;        // World * View * Projection matrix
}

// Textures
Texture2D         g_txScene     : register( t0 );

// Samplers
SamplerState                g_SampleLinear      : register( s0 );

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

PS_RenderOutput PS( PS_RenderSceneInput In )
{
    PS_RenderOutput O;

	O.Color = g_txScene.Sample(g_SampleLinear, In.TexCoord);

    return O;
}
