//PIxel shader input description
struct PS_INPUT 
{
	float4 Color : COLOR;
	float4 Pos : SV_POSITION;
	float4 WorldPos : TEXCOORD;
};

/*
Per frame constant buffer
*/
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 LightPosition;
}

//Once per app run constant buffer
cbuffer cbVertexDecals : register (b1)
{
	float4 decal[8];
	float4 dataStep;
}


//The samplerstate used to sample textures
SamplerState samplerPoint : register(s0);
SamplerState samplerTexture : register(s1);
//The texture containing the densities
Texture3D<float> densityTex : register(t0);
Texture2D textureX : register(t1);
Texture2D textureY : register(t2);
Texture2D textureZ : register(t3);

//Samples the texture and gets a value at the specific point
float getTexValue(float3 position)
{
	return densityTex.Sample(samplerPoint, position);
}

//Pixel shader main function
float4 main(PS_INPUT input) : SV_TARGET 
{
	//Gradient, AKA where in which direction within the scalar field the scalars change the most from this point. Here it is used to get an approximation of the surface's normal at this point.
	
    float3 gradient = float3(
		getTexValue((input.WorldPos.xyz + float3(dataStep.x, 0, 0) + 1.0f) / 2.0f) - getTexValue((input.WorldPos.xyz + float3(-dataStep.x, 0, 0) + 1.0f) / 2.0f),
		getTexValue((input.WorldPos.xyz + float3(0, dataStep.y, 0) + 1.0f) / 2.0f) - getTexValue((input.WorldPos.xyz + float3(0, -dataStep.y, 0) + 1.0f) / 2.0f),
		getTexValue((input.WorldPos.xyz + float3(0, 0, dataStep.z) + 1.0f) / 2.0f) - getTexValue((input.WorldPos.xyz + float3(0, 0, -dataStep.z) + 1.0f) / 2.0f)
	);

    float3 lightDir = normalize(LightPosition.xyz - input.WorldPos.xyz);
    float3 normal = -normalize(gradient);
    
    float3 blend_weights = abs(normal.xyz); // Tighten up the blending zone:  
    blend_weights = (blend_weights - 0.2) * 7;
    blend_weights = max(blend_weights, 0); // Force weights to sum to 1.0 (very important!)  
    blend_weights /= (blend_weights.x + blend_weights.y + blend_weights.z).xxx;
      
    //Scales the Texture size BIGGER MEANS BETTER
    float tex_scale = 5.0;

    float4 blended_color;  
   
    //Project the Geometry along one axis, XYZ respectivly and get the 2D coordinated from the projection
    float2 coord1 = input.WorldPos.yz * tex_scale;
    float2 coord2 = input.WorldPos.zx * tex_scale;
    float2 coord3 = input.WorldPos.xy * tex_scale;

    //Sample teture with the coordinates
    float4 col1 = textureX.Sample(samplerTexture, coord1);
    float4 col2 = textureY.Sample(samplerTexture, coord2);
    float4 col3 = textureZ.Sample(samplerTexture, coord3);

    blended_color = col1.xyzw * blend_weights.xxxx +
                col2.xyzw * blend_weights.yyyy +
                col3.xyzw * blend_weights.zzzz;
    

    // Light Calculations
	float4 lightIntensity = dot(lightDir, normal);
    float4 color = saturate(lightIntensity * blended_color);

	color = float4(color.xyz, 1);
	return color;
}