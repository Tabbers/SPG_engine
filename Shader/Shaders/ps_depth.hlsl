
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 linearDepth : LDEPTH;
    float4 fixedPointDepth : FPDEPTH;
};

float2 ComputMoments(float depth)
{
    float moment1 = depth;
    float moment2 = depth * depth;
	
		// Adjusting moments (this is sort of bias per pixel) using partial derivative
    float dx = ddx(depth);
    float dy = ddy(depth);
    moment2 += 0.25 * (dx * dx + dy * dy);
    return float2(moment1, moment2);
}

float4 main(PixelInputType input) : SV_TARGET
{
	float depth = 0.0f;
    float fixedPointShadows = 0.0f;
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depth = length(input.linearDepth);
    fixedPointShadows = input.fixedPointDepth.z / input.fixedPointDepth.w;
    return float4(ComputMoments(depth), fixedPointShadows, 1.0f);
}
