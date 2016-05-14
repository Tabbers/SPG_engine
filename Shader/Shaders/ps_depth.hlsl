
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};

float2 ComputeMoments(float Depth)
{
    float2 Moments;
  // First moment is the depth itself.
    Moments.x = Depth;
  // Compute partial derivatives of depth.
    float dx = ddx(Depth);
    float dy = ddy(Depth);
  // Compute second moment over the pixel extents.
    Moments.y = Depth * Depth + 0.25 * (dx * dx + dy * dy);
    return Moments;
}
float4 main(PixelInputType input) : SV_TARGET
{
	float depthValue;
	float4 color;
	
	
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depthValue = input.depthPosition.z / input.depthPosition.w;

    float2 moments = ComputeMoments(depthValue);

    color = float4(moments,depthValue, 1.0f);

	return color;
}
