//***********************************************************
// GLOBALS
//**********************************************************
cbuffer MatrixBuffer : register(b0)
{
    matrix g_WorldMatrix;
    matrix g_ViewMatrix;
    matrix g_ProjectionMatrix;
}

SamplerState g_HeightmapSampler : register(s0);
Texture2D g_Heightmap : register(t0);

//***********************************************************
// IN/OUT
//**********************************************************


struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor[2]			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 4
#define MAX_HEIGHT 10

[domain("quad")]
DS_OUTPUT main(	HS_CONSTANT_DATA_OUTPUT input, 	float2 domain : SV_DomainLocation,	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT OUT;

    //Map the Uniform quad uvs to the Actual quad uvs
    float2 uEdge = patch[1].uv - patch[0].uv;
    float2 vEdge = patch[3].uv - patch[2].uv;
    //Select the upper left corner as the origin
    float2 originUV = patch[0].uv;
    //map the uvs
    OUT.uv = originUV + (uEdge * domain.x) + (vEdge * domain.y);

     //Map the Uniform quad uvs to the Actual quad positions

    float2 xEdge = patch[1].position.xz - patch[0].position.xz;
    float2 yEdge = patch[3].position.xz - patch[2].position.xz;
    float2 originPos = patch[0].position.xz;

    //projected 3D Vertex onto a 2D planeto later add the height component
    float2 projectedVertexPostion = originPos + (xEdge - domain.x) + (yEdge - domain.y);

    float yValue = g_Heightmap.SampleLevel(g_HeightmapSampler,OUT.uv,0).r;
    float3 objectspaceVertexPosition = float3(projectedVertexPostion.x, yValue * MAX_HEIGHT, projectedVertexPostion.y);
  
    OUT.position = mul(float4(objectspaceVertexPosition, 1.0f), g_WorldMatrix);
    OUT.position = mul(OUT.position, g_ViewMatrix);
    OUT.position = mul(OUT.position, g_ProjectionMatrix);

	return OUT;
}
