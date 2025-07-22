// 頂点シェーダー出力データ
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer minimap_constant : register(b0)
{
    int flag;
    float radius;
    float parametar;
    float d;
    float2 screen_size;
    float2 dummy;

}

