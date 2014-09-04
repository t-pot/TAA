cbuffer cbConstants : register( b0 )
{
    float4x4 g_mWorldViewProjection;        // World * View * Projection matrix
}

//======================================================================================
// Textures and Samplers
//======================================================================================

// Textures
Texture2D         g_txScene     : register( t0 );

// Samplers
SamplerState                g_SampleLinear      : register( s0 );

//======================================================================================
// Vertex & Pixel shader structures
//======================================================================================

struct VS_RenderSceneInput
{
    float3 Position    : POSITION;  
    float3 Normal      : NORMAL;     
    float2 TexCoord    : TEXTURE0;
};

struct PS_RenderSceneInput
{
    float4 Position   : SV_Position;
    float4 Diffuse    : COLOR0; 
    float2 TexCoord   : TEXTURE0;
};

struct PS_RenderOutput
{
    float4 Color    : SV_Target0;
};

PS_RenderSceneInput VS_RenderScene( VS_RenderSceneInput In )
{
    PS_RenderSceneInput O;
    
    // Transform the position from object space to homogeneous projection space
    O.Position = mul( float4( In.Position, 1.0f ), g_mWorldViewProjection );
    O.Diffuse  = float4( dot( In.Normal, float3(0.7, 0.7, 0.3)).xxx, 1.0f );
    O.TexCoord = In.TexCoord;

    return O;    
}

PS_RenderOutput PS_RenderScene( PS_RenderSceneInput I )
{
    PS_RenderOutput O;

    O.Color =  ( saturate( float4( 0.3, 0.3, 0.3, 0.0 ) + 0.7 * In.Diffuse ) )
                  * g_txScene.Sample( g_SampleLinear, In.TexCoord );
    
    return O;
}
