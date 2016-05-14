struct INPUT
{
    float4 InitialPos : POSITION;
    float4 InitialVel : VELOCITY;
    float2 Size       : SIZE;
    float Age         : AGE;
    float Interval    : INTERVAL;
    unsigned int Type : TYPE;
};

INPUT main(INPUT input)
{
    return input;
}