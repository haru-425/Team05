#include "FullScreenQuad.hlsli"

#define LINEAR 1

Texture2D inputTex : register(t10);
SamplerState sampler_states[3] : register(s0);


// 疑似ランダム生成
float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// グレインノイズ
float GrainNoise(float2 uv, float time)
{
    float2 noiseUV = uv * 640.0 + time * 10.0;
    return rand(noiseUV);
}

// ホコリノイズ（大きめ）
float DustNoise(float2 uv, float time)
{
    float dust = 0.0;
    for (int i = 0; i < 3; ++i)
    {
        float scale = pow(1.8, i);
        float2 move = float2(sin(time * 0.1 + i * 5.0), cos(time * 0.15 + i * 8.0)) * 0.2;
        float2 dustUV = frac(uv * scale + move);
        float d = rand(dustUV);
        d = smoothstep(0.85, 1.0, d);
        dust += d * 0.5;
    }
    return saturate(dust);
}

// スキャンライン（改善版）
// - 横線が一定間隔で出現
// - 時間により下方向にスクロール
float Scanline(float2 uv, float time)
{
    float speed = 0.1; // スクロール速度
    float lineSpacing = 0.012; // ラインの間隔（1.0 = 画面全体）

    // UV.yにタイムオフセットを加えてラインをずらす
    float movingY = frac(uv.y + time * speed);

    // movingYがlineSpacingの倍数に近い場所でラインを出す
    float distanceToLine = fmod(movingY, lineSpacing);
    float mline = smoothstep(0.0, 0.0015, 0.001 - distanceToLine);

    return mline * 0.2; // 薄めに（強調したければ値を上げる）
}

// メインピクセルシェーダー
float4 main(VS_OUT input) : SV_TARGET
{
    float2 uv = input.texcoord;

    float4 color = inputTex.Sample(sampler_states[LINEAR], uv);

    float grain = GrainNoise(uv, iTime) * 0.05;
    float dust = DustNoise(uv, iTime) * 0.6;
    float scan = Scanline(uv, iTime); // 改善済みスキャンライン

    // 合成
    color.rgb = lerp(color.rgb, 1.0.xxx, dust * 0.35); // ホコリ
    color.rgb += grain; // グレイン
    color.rgb -= scan; // スキャンライン（暗い線）

    return saturate(color);
}

