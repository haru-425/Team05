#include "FullScreenQuad.hlsli"

#define CA_AMT 1.01
#define POINT 0
#define LINEAR 1

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

/**
 * @brief 289で割った余りを返す関数
 *
 * 値を289でラップすることで、パーミュテーションテーブルの範囲内に収めるために使用します。
 *
 * @param x 入力値
 * @return float 289でラップされた値
 */
float mod289(float x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

/**
 * @brief 289で割った余りを返す関数（float2バージョン）
 *
 * 2次元ベクトルの各成分を289でラップします。
 *
 * @param x 入力値（float2）
 * @return float2 289でラップされた値
 */
float2 mod289(float2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

/**
 * @brief 289で割った余りを返す関数（float3バージョン）
 *
 * 3次元ベクトルの各成分を289でラップします。
 *
 * @param x 入力値（float3）
 * @return float3 289でラップされた値
 */
float3 mod289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

/**
 * @brief 値をシャッフルするためのパーミュート関数
 *
 * ノイズ生成時の乱数インデックスを作るために使用します。
 *
 * @param x 入力値（float3）
 * @return float3 パーミュートされた値
 */
float3 permute(float3 x)
{
    return mod289(((x * 34.0) + 1.0) * x);
}

/**
 * @brief 2次元のシンプレックスノイズを生成する関数
 *
 * グラフィックエフェクトで自然な揺らぎやランダム性を表現するために使います。
 * Ian McEwan氏の実装をHLSLに移植したものです。
 *
 * @param v 入力座標（float2）
 * @return float シンプレックスノイズ値
 */
float snoise(float2 v)
{
    float4 C = float4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);

    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);

    float2 i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);

    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    i = mod289(i);
    float3 p = permute(permute(i.y + float3(0.0, i1.y, 1.0)) + i.x + float3(0.0, i1.x, 1.0));

    float3 m = max(0.5 - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m *= m * m * m;

    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

/**
 * @brief 乱数を生成する関数
 *
 * 入力座標から一様乱数を生成し、ノイズやエフェクトのバリエーションに利用します。
 *
 * @param co 入力座標（float2）
 * @return float 乱数値（0～1）
 */
float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

/**
 * @brief グリッチ効果を付与するピクセルシェーダのメイン関数
 *
 * ノイズや色収差、ランダムな色変化を組み合わせて、
 * 映像にグリッチ（乱れ）効果を与えます。
 *
 * @param pin 頂点シェーダから渡される出力構造体。テクスチャ座標(texcoord)などを含みます。
 * @return float4 最終的な色。アルファ値は常に1.0です。
 */
float4 main(VS_OUT pin) : SV_Target
{
    float2 uv = pin.texcoord;
    float time = iTime * 2.0;
    float NoiseStrength = 0.2; // 揺らぎの強さ（例：0.0 ～ 1.0）
    float NoiseScale = 6.0; // ノイズのスケール（細かさ）

    // 拡張：ノイズにスケールと強度を反映
    float baseNoise = max(0.0, snoise(float2(time, uv.y * 0.3 * NoiseScale)) - 0.3) * (1.0 / 0.7);
    baseNoise += (snoise(float2(time * 10.0, uv.y * 2.4 * NoiseScale)) - 0.5) * 0.15;
    float noise = baseNoise * NoiseStrength;

    float xpos = uv.x - noise * noise * 0.01;
    float4 color = texture_map.Sample(sampler_states[LINEAR], float2(xpos, uv.y));
    color.a = 1;

    float r = rand(uv.y * time);
    float3 redTint = float3(r, r, r);
    color.rgb = lerp(color.rgb, redTint, noise * 0.3);

    float g = lerp(color.r, texture_map.Sample(sampler_states[LINEAR], float2(xpos + noise * 0.05, uv.y)).g, 1);
    float b = lerp(color.r, texture_map.Sample(sampler_states[LINEAR], float2(xpos - noise * 0.05, uv.y)).b, 1);
    color.g = g;
    color.b = b;

    return color;
}
