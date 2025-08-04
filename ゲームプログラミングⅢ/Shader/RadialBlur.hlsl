#include "FullScreenQuad.hlsli"
// 入力テクスチャとサンプラ
Texture2D InputTexture : register(t0);
SamplerState InputSampler : register(s0);

// パラメータ
cbuffer RadialBlurParams : register(b0)
{
    float2 Center; // 中心座標（UV空間、例：float2(0.5, 0.5)）
    float BlurStrength; // ブラーの強さ（例：0.2）
    int SampleCount; // サンプル数（例：16）
    float FalloffPower; // フォールオフ指数（例：2.0）
    float2 DirectionBias; // 方向バイアス（例：float2(1.0, 0.5)）
};

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 dir = (pin.texcoord - Center) * DirectionBias;
    float2 step = dir * (BlurStrength / SampleCount);

    float4 accumulatedColor = float4(0, 0, 0, 0);
    float totalWeight = 0.0;

    for (int i = 0; i < SampleCount; ++i)
    {
        float t = (float) i / (SampleCount - 1);
        float weight = pow(t, FalloffPower); // 外側ほど重く

        float2 sampleUV = pin.texcoord - step * i;
        accumulatedColor += InputTexture.Sample(InputSampler, sampleUV) * weight;
        totalWeight += weight;
    }

    return accumulatedColor / totalWeight;
}
