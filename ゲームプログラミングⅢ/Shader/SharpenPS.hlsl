#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define SHARPEN_FACTOR 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);



// ピクセルシェーダのメイン関数
// sharpenMask関数で計算した色をそのまま出力
float4 main(VS_OUT pin) : SV_Target
{
    float3 center = texture_map.Sample(sampler_states[0], pin.texcoord).rgb;
    float2 texelSize = float2(1.0 / 1280.0, 1.0 / 720.0);
    float3 blur = (
    texture_map.Sample(sampler_states[0], pin.texcoord + float2(-texelSize.x, 0)).rgb +
    texture_map.Sample(sampler_states[0], pin.texcoord + float2(texelSize.x, 0)).rgb +
    texture_map.Sample(sampler_states[0], pin.texcoord + float2(0, -texelSize.y)).rgb +
    texture_map.Sample(sampler_states[0], pin.texcoord + float2(0, texelSize.y)).rgb

    ) * 0.25;
    float sharpness = SHARPEN_FACTOR;
    float3 result = center + sharpness * (center - blur);
    return float4(result, 1.0);


}