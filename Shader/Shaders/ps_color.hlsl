
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
    //Normals | Specular | Displacement | Hardshadows
    float4 Switches1;
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
    float4 lightViewPositionVSM : VSM;
	float3 lightPos          : TEXCOORD2;
	float4 worldposition	 : Position;
};

//========================================================
//DISPLACEMENT MAPPING
//========================================================
float2 Displacement(float2 tsEyeVec, float2 uv)
{
    int steps = int(stepsXRefiningStepsy.x);
    int refinementSteps = int(stepsXRefiningStepsy.y);

    float stepsize = 1 / float(steps);
    float refinementSize = 1 / float(refinementSteps);

    float2 NewCoords = uv;
    float2 dUV = -tsEyeVec.xy * 0.1 / float(steps);
    float SearchHeight = 1;

    float prev_hits = 0;
    float hit_h = 0;

    for (int it = 0; it < steps; it++)
    {
        SearchHeight -= stepsize;
        NewCoords += dUV;

        float CurrentHeight = displacementTexture.SampleLevel(SampleTypeWrap2, NewCoords, 0).x;
        float is_first_hit = saturate((CurrentHeight - SearchHeight - prev_hits) * 4999999);

        hit_h += is_first_hit * SearchHeight;
        prev_hits += is_first_hit;
    }

    
    NewCoords = uv + dUV * (1.0 - hit_h) * steps - dUV;
    
    float2 Temp = NewCoords;
    SearchHeight = hit_h + stepsize;
    float Start = SearchHeight;
    dUV *= refinementSize;
    prev_hits = 0.0;
    hit_h = 0.0;
    for (int i = 0; i < refinementSteps; i++)
    {
        SearchHeight -= stepsize * refinementSize;
        NewCoords += dUV;
        float CurrentHeight = displacementTexture.SampleLevel(SampleTypeWrap2, NewCoords, 0).x;
        float first_hit = saturate((CurrentHeight - SearchHeight - prev_hits) * 4999999);
        hit_h += first_hit * SearchHeight;
        prev_hits += first_hit;
    }
    NewCoords = Temp + dUV * (Start - hit_h) * steps * refinementSteps;
    
    return NewCoords;
}

//========================================================
//SHADOW MAPPING (VSM)
//========================================================

float2 SampleShadowMapXByY(float2 uv)
{
	float2 result = 0.0f;
	for (int x = -2; x < 3; ++x)
	{
		for (int y = -2; y < 3; ++y)
		{
			result += depthMapTexture.SampleLevel(SampleTypeClamp, uv, 0, int2(x,y)).xy;
		}
	}
	result /= 25;
	return result;
}
float linstep(float min, float max, float v)
{
    return clamp((v - min) / (max - min), 0.0f, 1.0f);
}

float ReduceLightBleeding(float p_max, float Amount)
{
  // Remove the [0, Amount] tail and linearly rescale (Amount, 1].  
    return linstep(Amount, 1, p_max);
}

float chebyshevUpperBound(float distance, float2 uv)
{
	// We retrive the two moments previously stored (depth and depth*depth)
    float2 moments = SampleShadowMapXByY(uv);
	// Surface is fully lit. as the current fragment is before the light occluder
    float p = distance <= moments.x;
		// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
		// How likely this pixel is to be lit (p_max)
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.2f);
	
    float d = distance - moments.x;
    float p_max = variance / (variance + d * d);

    p_max = ReduceLightBleeding(p_max, 0.2f);
    return max(p, p_max);
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
    float  lightIntensity;
    float4 colorOut = float4(0.0, 0.0, 0.0, 0.0);;
	float3 reflection;
    float2 lightTextCoords;
    float distanceToLight;
    float MinVariance = 0.00002f;
    float contribution;

    float4 ambient  = ambientColor;
    float4 diffuse  = float4(0.0, 0.0, 0.0, 0.0);
    float4 specular = float4(0.0, 0.0, 0.0, 0.0);

    uv = input.tex;
	if(Switches1.x == 1.0f)
	{
        if (Switches1.z == 1.0f)
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
    	
	// Calculate the projected texture coordinates.
	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w * 0.5 + 0.5f;
	projectTexCoord.y =  -input.lightViewPosition.y / input.lightViewPosition.w * 0.5 + 0.5f;

	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
        if(Switches1.w ==1.0f)
        {
           	float  lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
            depthValue = depthMapTexture.SampleLevel(SampleTypeClamp, projectTexCoord,0).z;
            lightDepthValue -= 0.00005f;
            if (lightDepthValue <= depthValue)
            {
		        // Calculate the amount of light on this pixel.
                lightIntensity = saturate(dot(float4(bumpNormal, 0.0), float4(input.lightPos, 1.0)));

                if (lightIntensity > 0.0f)
                {
		            // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                    diffuse = diffuseColor * lightIntensity;

		            // Saturate the final light color.
                    diffuse = saturate(diffuse);
                    if (Switches1.y == 1.0f)
                    {
			            //Calculate the reflection vector for the pixel
                        reflection = normalize(2 * lightIntensity * bumpNormal - lightDirection);

			            //calculate specular component
                        specular = pow(saturate(dot(reflection, input.viewDir)), specularIntensity);
                    }
		    // Combine the light and texture color.
                }
            }
        }
        else
        {   
            float lightDepthValue = length(input.lightViewPositionVSM);
            lightDepthValue -= 0.0002f;
            float ShadowContribution = chebyshevUpperBound(lightDepthValue, projectTexCoord);

		    // Calculate the amount of light on this pixel.
            lightIntensity = saturate(dot(float4(bumpNormal, 0.0), float4(input.lightPos, 1.0)));

            if (lightIntensity > 0.0f)
            {
		        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
                diffuse = diffuseColor * lightIntensity * ShadowContribution;

		        // Saturate the final light color.
                diffuse = saturate(diffuse);
                if (Switches1.y == 1.0f)
                {
			        //Calculate the reflection vector for the pixel
                    reflection = normalize(2 * lightIntensity * bumpNormal - lightDirection);

			        //calculate specular component
                    specular = pow(saturate(dot(reflection, input.viewDir)), specularIntensity);
                }
		    // Combine the light and texture color.
            }
        }

    }
    colorOut = textureColor * saturate(ambient + diffuse);
    return colorOut;
}
