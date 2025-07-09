#include "FullScreenQuad.hlsli"

Texture2D sceneTex : register(t10);
SamplerState samp : register(s0);

cbuffer FlickerParams : register(b13)
{
    float flickerSpeed; // ちらつき中のノイズ速度（例：40.0）
    float flickerStrength; // 明るさの揺らぎの強さ（例：0.4）
    float flickerDuration; // ちらつき状態が続く時間（秒）（例：1.0）
    float flickerChance; // ちらつきが起こる確率（例：0.05）
};


// 擬似ランダム
float rand(float x)
{
    return frac(sin(x * 12.9898) * 43758.5453);
}

// ノイズによるちらつき（高周波の明るさゆらぎ）
float getFlickerNoise(float time, float speed)
{
    float base = floor(time * speed);
    float t = frac(time * speed);
    float a = rand(base);
    float b = rand(base + 1.0);
    return lerp(a, b, t);
}

// "ちらつき状態に入っているかどうか" を判定
bool isInFlickerState(float time, float duration, float chance)
{
    float groupTime = floor(time / duration); // ちらつき候補ブロック
    float r = rand(groupTime);
    return r < chance;
}

float4 main(VS_OUT input) : SV_Target
{
    float2 uv = input.texcoord;
    float4 color = sceneTex.Sample(samp, uv);

    float brightness = 1.0;

    // 一定確率でちらつき状態になる
    if (isInFlickerState(iTime, flickerDuration, flickerChance))
    {
        float noise = getFlickerNoise(iTime, flickerSpeed);
        brightness = 1.0 - (noise * flickerStrength);
    }

    color.rgb *= brightness;
    return color;
}
