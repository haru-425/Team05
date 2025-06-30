struct VS_OUT
{
    float4 vertex         : SV_POSITION;
    float3 position       : POSITION;
    float2 texcoord       : TEXCOORD;
    float3 binormal       : BINORMAL;
    float3 normal         : NORMAL;
    float3 tangent        : TANGENT;
    float4 color          : COLOR;
    float3 shadowTexcoord : TEXCOORD1;
    float2 roughnessUV    : TEXCOORD2;
};