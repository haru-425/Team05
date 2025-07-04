#include "fullscreenquad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps : register(t10);
// テレビの砂嵐風ノイズ生成（UVベース）
float tv_noise(float2 uv)
{
    float y_pos = uv.y * iResolution.y;

    float grain = frac(sin(y_pos * 12.9898 + iTime * 78.233) * 43758.5453);
    float flicker = frac(sin(iTime * 100.0) * 12345.6789);

    return abs(grain - flicker);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float startTime = 2.0;
    float noiseDuration = 0.5;
    float waitDuration = 0.5;
    float fadeDuration = 0.0;

    float time = max(iTime - startTime, 0.0);

// ノイズ強度計算
    float noise_strength = saturate(time / noiseDuration);

// フェード制御（ノイズ完了後、少し待ってからフェード）
    float fade = (time > noiseDuration + waitDuration) ?
    1.0 - saturate((time - noiseDuration - waitDuration) / fadeDuration) : 1.0;

    float4 original = texture_maps.Sample(sampler_states[LINEAR], pin.texcoord);
    float noise = tv_noise(pin.texcoord); // 横線風ノイズ

    float3 storm = lerp(original.rgb, float3(noise, noise, noise), noise_strength);
    return float4(storm * fade, original.a);
}
