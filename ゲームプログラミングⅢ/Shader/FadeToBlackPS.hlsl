#include "FullScreenQuad.hlsli"

#define LINEAR 1

// 入力テクスチャ
Texture2D texture_map : register(t10);
SamplerState sampler_states[3] : register(s0);

// 定数バッファ：フェード強度
cbuffer FadeCBuffer : register(b0)
{
    float FadeAmount; // 0.0 = 表示, 1.0 = 真っ黒
}

float4 main(VS_OUT pin) : SV_Target
{
    float4 color = texture_map.Sample(sampler_states[LINEAR], pin.texcoord);
    float FadeDuration = 1.0;
    float fade = saturate(signalTime / FadeDuration);
    // 黒との線形補間
    color.rgb = lerp(color.rgb, float3(0, 0, 0), fade);

    return color;
}
