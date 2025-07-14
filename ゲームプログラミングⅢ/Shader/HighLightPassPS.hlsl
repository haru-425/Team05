#include "FullScreenQuad.hlsli"
#define POINT 0
#define LINEAR 1
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t10);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_maps[POINT].Sample(sampler_states[LINEAR], pin.texcoord);
    float alpha = color.a;
    color.rgb = step(0.8, dot(color.rgb, float3(0.299, 0.587, 0.114))) * color.rgb;
    return float4(color.rgb, alpha);
}
