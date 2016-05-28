//***********************************************************
// GLOBALS
//**********************************************************
SamplerState g_TextureSampler : register(s0);
Texture2D g_diffuse : register(t0);
//***********************************************************
// IN/OUT
//**********************************************************
struct PIXEL_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PIXEL_INPUT IN) : SV_TARGET
{
    float4 OUT;

    OUT = g_diffuse.Sample(g_TextureSampler, IN.uv);

    return OUT;
}