Texture2D colorMap_ : register(t0);
SamplerState colorSampler_ : register(s0);

cbuffer cbChangesPerFrame : register(b0)
{
    float4x4 transform_;
};

struct VS_Input
{
    float3 pos  : POSITION;
    float2 tex0 : TEXCOORDS;
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float2 tex0 : TEXCOORDS;
};

PS_Input VS_Main( VS_Input vertex )
{
    float4 pos = float4(vertex.pos, 1.0f);
    PS_Input vsOut;
    vsOut.pos = mul(transform_, pos);
    vsOut.tex0 = vertex.tex0;
    return vsOut;
}

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    float2 texCoords = frag.tex0;
    texCoords.y = 1.0f - texCoords.y;

    float4 color = colorMap_.Sample( colorSampler_, texCoords );
    return float4(1.0f, 1.0f, 1.0f, color.x);
}