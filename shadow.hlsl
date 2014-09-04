cbuffer cbConstants : register( b0 )
{
    float4x4 g_mWorldViewProjection;        // World * View * Projection matrix
}

//======================================================================================
// Vertex & Pixel shader structures
//======================================================================================

struct VS_INPUT
{
    float3 Position    : POSITION;  
};

struct VS_OUTPUT
{
    float4 Position   : SV_Position;
};

struct PS_OUTPUT
{
    float4 Color    : SV_Target0;
};

//======================================================================================
// Shaders
//======================================================================================

VS_OUTPUT VSMain( VS_INPUT In )
{
    VS_OUTPUT O;

    O.Position = mul( float4( In.Position, 1.0f ), g_mWorldViewProjection );

    return O;    
}

PS_OUTPUT PSMain( VS_OUTPUT In )
{
    PS_OUTPUT O;

    O.Color.rgba = In.Position.z / In.Position.w;

    return O;
}
