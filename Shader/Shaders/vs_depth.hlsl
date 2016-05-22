
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 linearDepth : LDEPTH;
    float4 fixedPointDepth : FPDEPTH;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
 
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.fixedPointDepth = output.position;

	// Store the position value in a second input value for depth value calculations.
    output.linearDepth = mul(input.position, worldMatrix);
    output.linearDepth = mul(output.linearDepth, viewMatrix);
	
	return output;
}