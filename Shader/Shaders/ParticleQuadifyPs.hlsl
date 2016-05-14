
Texture2D objectTexture : register(t0);
SamplerState SampleAnisotropicWrap : register(s0);


struct GS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
    uint type : TYPE;
};
float4 main(GS_INPUT input) : SV_TARGET
{
	float4 outColor;
    outColor = objectTexture.Sample(SampleAnisotropicWrap, input.tex) * input.color;
    return outColor;
}