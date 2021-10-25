Texture2D colorMap_ : register(t0);
SamplerState colorSampler_ : register(s0);

struct VS_Input
{
    float4 pos  : POSITION;
    float2 tex0 : TEXCOORDS;
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float2 tex0 : TEXCOORDS;
};

PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vsOut.pos = vertex.pos;
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