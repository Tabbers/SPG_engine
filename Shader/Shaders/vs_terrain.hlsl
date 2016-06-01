//***********************************************************
// IN/OUT
//**********************************************************
struct VERTEX_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
};
struct VERTEX_OUTPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
};

VERTEX_OUTPUT main( VERTEX_INPUT IN )
{
    VERTEX_OUTPUT OUT;
    
    OUT.position = IN.position;
    OUT.tex = IN.tex;
	
    return OUT;
}