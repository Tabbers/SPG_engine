cbuffer MatrixBuffer : register(b0)
{

    matrix World;
    matrix View;
    matrix Projection;
}

//Once per app run constant buffer
cbuffer DataBuffer : register(b1)
{
    float4 data;
}

struct GS_INPUT
{
    float4 InitialPos : POSITION;
    float4 InitialVel : VELOCITY;
    float2 Size : SIZE;
    float Age : AGE;
    float Interval : INTERVAL;
    unsigned int Type : TYPE;
};

struct GS_OUTPUT
{
	float4 pos   : SV_POSITION;
    float4 color : COLOR;
    float2 tex   : TEXCOORD;
    uint type    : Type;
};

struct Type
{
    uint EMITTER;
    uint FLYING;
    uint FALLING;
};


[maxvertexcount(4)]
void main( point GS_INPUT gin[1],	inout TriangleStream< GS_OUTPUT > output)
{
    Type t;
    t.EMITTER = 0;
    t.FLYING = 1;
    t.FALLING = 2;

    float4x4 viewproj = mul(View, Projection);

    float2 QuadTexC[4] =
    {
        float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
    };

	if(gin[0].Type != t.EMITTER)
    {
        //
		// Compute world matrix so that billboard faces the camera.
		//
        float3 look = normalize(data.xyz - gin[0].InitialPos);
        float3 right = normalize(cross(float3(0, 1, 0), look));
        float3 up = cross(look, right);
		
		//
		// Compute triangle strip vertices (quad) in world space.
		//
        float halfWidth = 0.5f * gin[0].Size.x;
        float halfHeight = 0.5f * gin[0].Size.y;
	
        float4 v[4];
        v[0] = float4(gin[0].InitialPos + halfWidth * right - halfHeight * up, 1.0f);
        v[1] = float4(gin[0].InitialPos + halfWidth * right + halfHeight * up, 1.0f);
        v[2] = float4(gin[0].InitialPos - halfWidth * right - halfHeight * up, 1.0f);
        v[3] = float4(gin[0].InitialPos - halfWidth * right + halfHeight * up, 1.0f);
		

 		//
		// Transform quad vertices to world space and output 
		// them as a triangle strip.
		//
        GS_OUTPUT gout;
        for (int i = 0; i < 4; ++i)
        {
			float4 position = mul(v[i], View);
			position = mul(position, Projection);
			gout.pos = position;
            gout.tex = QuadTexC[i];
            gout.color = float4(1.0f / 20, 1.0f / 20, 1.0f / 20, 1.0f);
            gout.type = gin[0].Type;
            output.Append(gout);
        }
    }
}