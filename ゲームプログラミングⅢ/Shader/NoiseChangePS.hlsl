// フルスクリーンクワッド用の入力
#include "FullScreenQuad.hlsli"

Texture2D sceneTex : register(t10);
SamplerState samp : register(s0);

float LineNoise(float y)
{
    float Nline = floor(y * 300.0);
    return frac(sin(Nline + iTime * 20.0) * 43758.5453);
}

float4 main(VS_OUT pin) : SV_Target
{
    float TriggerInterval = 5.0;
    float TransitionDuration = 0.5;

    float2 uv = pin.texcoord;
    float4 baseColor = sceneTex.Sample(samp, uv);

    if (iTime < TriggerInterval)
    {
        return baseColor;
    }

    float cycleTime = fmod(iTime - TriggerInterval, TriggerInterval);
    float strength = 0.0f;

    if (cycleTime < TransitionDuration)
    {
        float t = cycleTime / TransitionDuration;
        strength = sin(t * 3.14159); // 滑らかなフェード
    }

    // ノイズ生成
    float noise = LineNoise(uv.y);

    // 暗い背景への馴染みを考慮したノイズカラー補正
    float3 tint = float3(0.1, 0.05, 0.0); // 暖色寄りのベーストーン
    float3 noiseColor = lerp(tint, float3(noise, noise, noise), 0.7);

    // 背景の輝度を元に合成強度を調整
    float luminance = dot(baseColor.rgb, float3(0.299, 0.587, 0.114));
    float contrastFactor = saturate(1.0 - luminance);

    float blend = strength * contrastFactor;

    // 合成：暗い背景にも違和感なくノイズが溶け込む
    float3 finalColor = lerp(baseColor.rgb, noiseColor, blend);

    return float4(finalColor, 1.0);
}