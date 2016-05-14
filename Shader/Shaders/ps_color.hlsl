
Texture2D depthMapTexture     : register(t0);
Texture2D objectTexture       : register(t1);
Texture2D normalTexture       : register(t2);
Texture2D displacementTexture : register(t3);

SamplerState SampleTypeClamp  : register(s0);
SamplerState SampleTypeWrap   : register(s1);
SamplerState SampleTypeWrap2  : register(s2);

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
	float4 specularColor;
	float3 lightDirection;
	float  specularIntensity;
	bool   drawNormal;
	float3 padding;
	bool   drawSpec;
	float3 padding2;
    bool   drawDisp;
    float3 padding3;
    float2 stepsXRefiningStepsy;
    float2 padding5;
	
};
struct PixelInputType
{
    float4 position          : SV_POSITION;
    float2 tex               : TEXCOORD0;
	float3 normal            : NORMAL;
	float3 viewDir           : TEXCOORD3;
	float3 tangent           : TANGENT;
	float3 binormal          : BINORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos          : TEXCOORD2;
};

float ChebyshevUpperBound(float2 Moments, float t, float minVariance)
{
  // One-tailed inequality valid if t > Moments.x  
    float p = (t <= Moments.x);
  // Compute variance.  
    float Variance = (Moments.x * Moments.x) - Moments.y;
    Variance = max(Variance, minVariance);
  // Compute probabilistic upper bound.  
    float d = Moments.x - t;
    float p_max = Variance / (Variance + d * d);
    return max(p, p_max);
}
float ShadowContribution(float2 LightTexCoord, float DistanceToLight, float minVariance)
{
  // Read the moments from the variance shadow map.  
    float2 Moments = depthMapTexture.Sample(SampleTypeClamp, LightTexCoord).xy;
  // Compute the Chebyshev upper bound.  
    return ChebyshevUpperBound(Moments, DistanceToLight, minVariance);
}

float2 Displacement(float2 tsEyeVec, float2 uv)
{
    int steps = int(stepsXRefiningStepsy.x);
    int refinementSteps = int(stepsXRefiningStepsy.y);

    float stepsize = 1 / float(steps);
    float refinementSize = 1 / float(refinementSteps);

    float2 NewCoords = uv;
    float2 dUV = -tsEyeVec.xy * 0.1 / float(steps);
    float SearchHeight = 1;

    float  prev_hits = 0; 
    float  hit_h     = 0;

    for (int it=0; it < steps; it++) 
    { 
        SearchHeight -= stepsize;
        NewCoords    += dUV; 

        float CurrentHeight = displacementTexture.SampleLevel(SampleTypeWrap2, NewCoords,0).x;
        float is_first_hit  = saturate((CurrentHeight - SearchHeight - prev_hits) * 4999999);

        hit_h     += is_first_hit * SearchHeight;
        prev_hits += is_first_hit; 
    }

    
    NewCoords = uv + dUV * (1.0 - hit_h) * steps -dUV;
    
    float2 Temp = NewCoords;
    SearchHeight = hit_h + stepsize;
    float Start = SearchHeight;
    dUV *= refinementSize;
    prev_hits = 0.0;
    hit_h = 0.0;
    for (int i = 0; i < refinementSteps; i++)
    {
        SearchHeight -= stepsize * refinementSize;
        NewCoords    += dUV;
        float CurrentHeight = displacementTexture.SampleLevel(SampleTypeWrap2, NewCoords,0).x;
        float first_hit     = saturate((CurrentHeight - SearchHeight - prev_hits) * 4999999);
        hit_h     += first_hit * SearchHeight;
        prev_hits += first_hit;
    }
    NewCoords = Temp + dUV * (Start - hit_h) * steps* refinementSteps;
    
    return NewCoords;
}

float4 main(PixelInputType input) : SV_TARGET
{
    float2 uv;
	float  bias;
	float4 textureColor;
	float4 normalMapColor;
	float3 bumpNormal;
    float2 tsEyevec;
	float2 projectTexCoord;
	float  depthValue;
	float  lightDepthValue;
    float  lightIntensity;
    float4 colorOut;
	float3 reflection;
	float4 specular;
    float2 lightTextCoords;
    float distanceToLight;
    float MinVariance = 0.00001f;
	specular = float4(0.0f,0.0f,0.0f,0.0f);
    float contribution = 1.0f;
    uv = input.tex;
	if(drawNormal)
	{
        if (drawDisp)
        {
               
            tsEyevec.x = dot(input.viewDir, input.tangent);
            tsEyevec.y = dot(input.viewDir, input.binormal);

            //tsEyevec = normalize(tsEyevec);

            uv = Displacement(tsEyevec, input.tex);
        }
		//Sample NormalMap
        normalMapColor = normalTexture.SampleLevel(SampleTypeWrap, uv,0);

		// Expand the range of the normal value from (0, +1) to (-1, +1).
		normalMapColor = (normalMapColor * 2.0f) - 1.0f;
    
		// Calculate the normal from the data in the bump map.
		bumpNormal = (normalMapColor.x * input.tangent) + (normalMapColor.y * input.binormal) + (normalMapColor.z * input.normal);
	
		// Normalize the resulting bump normal.
		bumpNormal = normalize(bumpNormal);


    }
	else
	{
		bumpNormal = input.normal;
        uv = input.tex;
    }
    //Sample Texture
    textureColor = objectTexture.SampleLevel(SampleTypeWrap, uv,0);

	// Set the bias value for fixing the floating point precision issues.
	bias = 0.00002f;

	// Set the default output color to the ambient light value for all pixels.
    colorOut = ambientColor;
	
	// Calculate the projected texture coordinates.
	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
        distanceToLight = length(input.lightPos - input.position.xyz);
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

        contribution = ShadowContribution(projectTexCoord, distanceToLight, MinVariance);
		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if(lightDepthValue < depthValue)
		{
		    // Calculate the amount of light on this pixel.
        lightIntensity = saturate(dot(bumpNormal, input.lightPos));

		    if(lightIntensity > 0.0f)
			{
				// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
				colorOut += (diffuseColor * lightIntensity);

				// Saturate the final light color.
				colorOut = saturate(colorOut);

				//Calculate the reflection vector for the pixel
				reflection = normalize(2 * lightIntensity * bumpNormal - lightDirection); 

				//calculate specular component
				specular = pow(saturate(dot(reflection, input.viewDir)), specularIntensity);
				// Combine the light and texture color.
			}
		}
	}
	colorOut = textureColor*colorOut;
	if(drawSpec) colorOut  = saturate(colorOut + specular);
    return colorOut * contribution;
}
