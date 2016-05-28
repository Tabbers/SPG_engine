//***********************************************************
// GLOBALS
//**********************************************************
cbuffer TesselationFactors: register(b0)
{
    float4 edgeFactors;
    float2 innerFactors;
};

//***********************************************************
// IN/OUT
//**********************************************************

// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor[2]		: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 4

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// Insert code to compute Output here
    Output.EdgeTessFactor[0] = edgeFactors.x;
    Output.EdgeTessFactor[1] = edgeFactors.y;
    Output.EdgeTessFactor[2] = edgeFactors.z;
    Output.EdgeTessFactor[3] = edgeFactors.w;

    Output.InsideTessFactor[0] = innerFactors.x;
    Output.InsideTessFactor[1] = innerFactors.y;
	return Output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main(InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> IN, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT OUT;

	// Insert code to compute Output here
    OUT.position = IN[i].position;
    OUT.tex = IN[i].tex;

	return OUT;
}
