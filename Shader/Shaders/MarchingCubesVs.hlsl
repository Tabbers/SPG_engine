
struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
};

//Vertex shader output description
struct VS_OUTPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR;
};

//Main vertex shader
VS_OUTPUT main(VS_INPUT IN )
{
	VS_OUTPUT output;
	output.Pos = float4(IN.Pos.xyz, 1);
    output.Color = IN.Color;
	return output;
}