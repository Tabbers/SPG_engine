cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer LightBuffer2
{
    float3 lightPosition;
	float padding;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding1;
    float3 cameraDirection;
    float padding2;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDir: TEXCOORD3;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
	float4 worldposition : Position;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
	output.worldposition = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// Store the input color for the pixel shader to use.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	// Calculate the normal vector against the world matrix only.
    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    // Normalize the normal vector.
    output.binormal = normalize(output.binormal);

	// Calculate the normal vector against the world matrix only.
    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    // Normalize the normal vector.
    output.tangent = normalize(output.tangent);

	// Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the light position based on the position of the light and the position of the vertex in the world.
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    // Normalize the light position vector.
    output.lightPos = normalize(output.lightPos);

	// Get the view Direction and possible Reflections
    worldPosition = mul(input.position, worldMatrix);
	//Generate an view vector based on camera position and world position of the vertex
    output.viewDir = cameraPosition - worldPosition;
    output.viewDir = normalize(output.viewDir);

    return output;
}