//#include "FullScreenQuad.hlsli"
//// 入力テクスチャとサンプラ
//Texture2D InputTexture : register(t10);
//SamplerState InputSampler : register(s0);
//
//// パラメータ
//cbuffer RadialBlurParams : register(b9)
//{
//    float2 Center; // 中心座標（UV空間、例：float2(0.5, 0.5)）
//    float BlurStrength; // ブラーの強さ（例：0.2）
//    int SampleCount; // サンプル数（例：16）
//    float FalloffPower; // フォールオフ指数（例：2.0）
//    float2 DirectionBias; // 方向バイアス（例：float2(1.0, 0.5)）
//    float padding; // パディング
//};
//
//float4 main(VS_OUT pin) : SV_TARGET
//{
//    float2 dir = (pin.texcoord - Center) * DirectionBias;
//    float2 step = dir * (BlurStrength / SampleCount);
//
//    float4 accumulatedColor = float4(0, 0, 0, 0);
//    float totalWeight = 0.0;
//
//    for (int i = 0; i < SampleCount; ++i)
//    {
//        float t = (float) i / (SampleCount - 1);
//        float weight = pow(t, FalloffPower); // 外側ほど重く
//
//        float2 sampleUV = pin.texcoord - step * i;
//        accumulatedColor += InputTexture.Sample(InputSampler, sampleUV) * weight;
//        totalWeight += weight;
//    }
//
//    return accumulatedColor / totalWeight;
//}

#include "FullScreenQuad.hlsli"
Texture2D InputTexture : register(t10);
SamplerState InputSampler : register(s0);

cbuffer RadialBlurParams : register(b9)
{
    float2 Center; // ブラー中心（UV）
    float BlurStrength; // ブラーの強さ
    int SampleCount; // サンプル数
    float FalloffPower; // フォールオフ強度（指数）
    float2 DirectionBias; // 方向バイアス
    float OffsetStrength; // サンプル開始点を中心からずらす量（0?1）
    float CenterFeather; // 中心ぼかし軽減（0?1）
    float BlendAmount; // 元画像との混合比（0?1）
};

float4 main(VS_OUT pin) : SV_Target
{
    float2 uv = pin.texcoord;
    float2 dir = (uv - Center) * DirectionBias;
    float len = length(dir);

    // 中心近くでのブラーを減衰
    float feather = 1.0 - saturate(len * CenterFeather);

    // ステップ方向計算
    float2 step = dir * (BlurStrength * feather / SampleCount);

    float2 offsetStart = step * OffsetStrength; // 中心からの初期オフセット

    float4 accumulatedColor = float4(0, 0, 0, 0);
    float totalWeight = 0.0;

    for (int i = 0; i < SampleCount; ++i)
    {
        float t = (float) i / (SampleCount - 1);
        float weight = pow(t, FalloffPower);

        float2 sampleUV = uv - step * i - offsetStart;
        accumulatedColor += InputTexture.Sample(InputSampler, sampleUV) * weight;
        totalWeight += weight;
    }

    float4 blurred = accumulatedColor / totalWeight;

    // 元画像とブラーのブレンド
    float4 original = InputTexture.Sample(InputSampler, uv);
    return lerp(original, blurred, BlendAmount);
}