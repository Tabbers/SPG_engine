
SamplerState samLinear : register(s0);
Texture1D randomTex : register(t0);

cbuffer MatrixBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

//Once per app run constant buffer
cbuffer DataBuffer : register(b1)
{
    	//Gametime|garvity|initalVelocity|maximumLifetime
    float4 data;
    float4 Random;
}

struct Particle
{
    float4  InitialPos : POSITION;
    float4  InitialVel : VELOCITY;
    float2  Size       : SIZE;
    float   Age        : AGE;
    float   Interval   : INTERVAL;
    unsigned int Type  : TYPE;
};

struct Type
{
    uint EMITTER;
    uint FLYING;
    uint FALLING;
};

// Input: It usestexture coords as the random numberseed.
// Output: Random number: [0,1), that is between 0.0 and 0.999999... inclusive.
// Author: Michael Pohoreski
// Copyright: Copyleft 2012 ??
float random(float2 p)
{
  // We need irrationals for pseudo randomness.
  // Most (all?) known transcendental numbers will (generally) work.
    float2 r = float2(
    23.1406926327792690, // e^pi (Gelfond's constant)
    2.6651441426902251); // 2^sqrt(2) (Gelfond–Schneider constant)
    int dummy = 0;
    return modf(cos(fmod(123456789., 1e-7 + 256. * dot(p, r))), dummy);
}

float3 RandUnitVec3(float offset)
{
    float u = (data.x + offset);
    float3 v = randomTex.SampleLevel(samLinear, u, 0).xyz;
    return normalize(v);
}

[maxvertexcount(21)]
void main(point Particle input[1], inout PointStream<Particle> output, uint id : SV_PrimitiveID)
{
    Type t;
    t.EMITTER = 0;
    t.FLYING = 1;
    t.FALLING = 2;

    input[0].Age += data.x;
	if(input[0].Type == t.EMITTER)
    {
        bool emit = true;
        //Put the Emitter back in the StreamOutput
        Particle emitter;

        emitter = input[0];
        if (input[0].Age >= input[0].Interval)
        {
            emit = true;
            emitter.Age = 0.0f;
        }
        output.Append(emitter);

        if(emit)
        {

            for (int i = 0; i <20; ++i)
            {
                float3 vRandom;
               vRandom.x = random(float2((Random.x + id) * 0.5, (Random.y + id) * 0.25));
                vRandom.y = random(float2((Random.z + id) * 0.625, (Random.x + id) * 0.25));
                vRandom.z = random(float2((Random.y + id) * 0.125, (Random.z + id) * 0.5));
                
            //Add a new Emitted Particle
                Particle newParticle;
                newParticle.InitialPos = input[0].InitialPos;
                newParticle.InitialVel = float4(vRandom.xyz, 0.0f);
                newParticle.Size = float2(1.0f, 1.0f);
                newParticle.Age = 0;
                newParticle.Type = t.FLYING;
                newParticle.Interval = 0.0f;
                output.Append(newParticle);
            }
       }
    }
    else if(input[0].Type == t.FLYING)
    {
        if (input[0].Age < (data.w/2))
        {
            Particle newParticle;
            newParticle.InitialPos = input[0].InitialPos + input[0].InitialVel * data.x + input[0].InitialVel * ((data.x * data.x) * 0.5);
            newParticle.InitialVel = input[0].InitialVel;
            newParticle.Size = input[0].Size;
            newParticle.Age = input[0].Age + data.x;
            newParticle.Type = input[0].Type;
            newParticle.Interval = 0.0f;

            output.Append(newParticle);
        }
        else
        {
            float4 fallingVector = input[0].InitialVel;
            fallingVector.y = -data.z;

            Particle newParticle;
            newParticle.InitialPos = input[0].InitialPos + input[0].InitialVel * data.x + input[0].InitialVel * ((data.x * data.x) * 0.5);
            newParticle.InitialVel = fallingVector;
            newParticle.Size = float2(input[0].Size.x / 20, input[0].Size.y);
            newParticle.Age = input[0].Age + data.x;
            newParticle.Type = t.FALLING;
            newParticle.Interval = 0.0f;

            output.Append(newParticle);
        }
    }
    else if (input[0].Type == t.FALLING)
    {
        if (input[0].Age < data.w)
        {
            Particle newParticle;
            newParticle.InitialPos = input[0].InitialPos + input[0].InitialVel * data.x + input[0].InitialVel * ((data.x * data.x) * 0.5);
            newParticle.InitialVel = input[0].InitialVel;
            newParticle.Size = input[0].Size;
            newParticle.Age = input[0].Age + data.x;
            newParticle.Type = input[0].Type;
            newParticle.Interval = 0.0f;

            output.Append(newParticle);
        }
    }

}