#include "FullScreenQuad.hlsli"

Texture2D sceneTex : register(t10);
SamplerState samp : register(s0);

cbuffer FlickerParams : register(b13)
{
    float flickerSpeed; // ノイズベースのちらつき速度（例: 20.0）
    float flickerStrength; // ノイズによる明るさの振れ幅（例: 0.4）
    float flashInterval; // 大きな暗転の周期（例: 4.0秒）
    float flashDuration; // 暗転時間（例: 0.2秒）
};


// 擬似ランダム関数
float rand(float x)
{
    return frac(sin(x * 12.9898) * 43758.5453);
}

// パーリンノイズ風ちらつき（高速明滅）
float getFlickerNoise(float time, float speed)
{
    float base = floor(time * speed);
    float fracPart = frac(time * speed);
    float n0 = rand(base);
    float n1 = rand(base + 1.0);
    return lerp(n0, n1, fracPart);
}

// 蛍光灯のような大きな暗転判定
float getFlash(float time, float interval, float duration)
{
    float cycleTime = fmod(time, interval);
    return (cycleTime < duration) ? 0.0 : 1.0;
}


float4 main(VS_OUT input) : SV_Target
{
    float2 uv = input.texcoord;
    float4 color = sceneTex.Sample(samp, uv);

    // 高周波ノイズで明るさを細かく揺らす
    float flicker = getFlickerNoise(iTime, flickerSpeed);
    float brightness = 1.0 - (flicker * flickerStrength);

    // 一定周期で完全に暗転するフェーズを入れる
    float flash = getFlash(iTime, flashInterval, flashDuration);

    // フラッシュによる強制消灯とノイズを合成
    brightness *= flash;

    color.rgb *= brightness;
    return color;
}