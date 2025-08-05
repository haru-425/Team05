#include "fullscreenquad.hlsli"
// VHSPS.hlsl
// VHS風のエフェクトを適用するピクセルシェーダー
// 入力テクスチャとサンプラ
Texture2D InputTexture : register(t10);
SamplerState InputSampler : register(s0);
float4 hash42(float2 p)
{
    float4 p4 = frac(float4(p.xyxy) * float4(443.8975, 397.2973, 491.1871, 470.7827));
    p4 += dot(p4.wzxy, p4 + 19.19);
    return frac(float4(p4.x * p4.y, p4.x * p4.z, p4.y * p4.w, p4.x * p4.w));
}

float hash(float n)
{
    return frac(sin(n) * 43758.5453123);
}

float n(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);

    float n = p.x + p.y * 57.0 + 113.0 * p.z;

    float res = lerp(lerp(lerp(hash(n + 0.0), hash(n + 1.0), f.x),
                          lerp(hash(n + 57.0), hash(n + 58.0), f.x), f.y),
                     lerp(lerp(hash(n + 113.0), hash(n + 114.0), f.x),
                          lerp(hash(n + 170.0), hash(n + 171.0), f.x), f.y), f.z);

    return res;
}

float nn(float2 p)
{
    float y = p.y;
    float s = iTime * 2.0;

    float v = (n(float3(y * 0.01 + s, 1.0, 1.0)))
            * (n(float3(y * 0.011 + 1000.0 + s, 1.0, 1.0)))
            * (n(float3(y * 0.51 + 421.0 + s, 1.0, 1.0)));

    v *= hash42(float2(p.x + iTime * 0.01, p.y)).x + 0.3;
    v = pow(v + 0.3, 1.0);
    if (v < 0.7)
        v = 0.0;

    return v;
}


float4 main(VS_OUT pin) : SV_TARGET
{
   // float4 color = InputTexture.Sample(InputSampler, pin.texcoord);
    float2 fragCoord = pin.position;

    float2 uv = pin.texcoord;

    float linesN = 240.0;
    float one_y = iResolution.y / linesN;
    uv = floor(uv * iResolution / one_y) * one_y;

    float col = nn(uv);
    float offset = col * 0.01; // ノイズによる横シフト
    float2 shiftedUV = pin.texcoord + float2(offset, 0.0);
    float4 color = InputTexture.Sample(InputSampler, shiftedUV);

    color.a = 1.0; // Ensure alpha is set to 1 for the final output
    return color;
    return float4(col, col, col, 1.0);
}