// フルスクリーンクワッド用の入力
#include "FullScreenQuad.hlsli"


Texture2D sceneTex : register(t10);
SamplerState samp : register(s0);

float LineNoise(float y)
{
    // 画面の高さ方向を N 本の水平ラインに丸める
    float Nline = floor(y * 300.0); // ラインインデックス
    return frac(sin(Nline + iTime * 20.0) * 43758.5453);
}

// メインピクセルシェーダー
float4 main(VS_OUT pin) : SV_Target
{
    float TriggerInterval = 5.0; // ノイズ発生間隔（秒）
    float TransitionDuration = 0.5; // フェードイン・アウト時間（秒）

    float2 uv = pin.texcoord;
    float cycleTime = fmod(iTime, TriggerInterval);

    float strength = 0.0f;
    if (cycleTime < TransitionDuration)
    {
        float t = cycleTime / TransitionDuration;
        strength = sin(t * 3.14159); // 滑らかに 0 → 1 → 0
    }

    // ベースカラー
    float4 baseColor = sceneTex.Sample(samp, uv);

    // 横線ノイズ生成
    float noise = LineNoise(uv.y);
    float4 noiseColor = float4(noise, noise, noise, 1.0);

    // ノイズをフェードで合成
    return lerp(baseColor, noiseColor, strength);
}
