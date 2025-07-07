#include "FullScreenQuad.hlsli"

#define LINEAR 1

Texture2D texture_map : register(t10);
SamplerState sampler_states[3] : register(s0);

// ラインベースのノイズ（縦方向のラインが時間でランダムにずれる）
float LineNoise(float y, float time)
{
    float offset = frac(sin(time * 10.0) * 1000.0);
    return frac(sin(dot(float2((y + offset) * 240.0, time), float2(12.9898, 78.233))) * 43758.5453);
}

float4 main(VS_OUT pin) : SV_Target
{
    float2 uv = pin.texcoord;

    //----------------------------------
    float4 sceneColor = texture_map.Sample(sampler_states[LINEAR], uv);

    //----------------------------------
    float offset = sin(uv.y * 80.0 + iTime * 8.0) * 0.008;
    uv.x += offset;

    //----------------------------------
    float3 color = texture_map.Sample(sampler_states[LINEAR], uv).rgb;

    //----------------------------------
    float lineNoiseY = LineNoise(uv.y + iTime * 0.2, iTime);
    float3 noiseColorY = (lineNoiseY - 0.5).xxx * 1.2; // 強調！

    //----------------------------------
    float lineNoiseX = frac(sin(dot(float2(uv.x * 240.0, iTime), float2(12.9898, 78.233))) * 43758.5453);
    float3 noiseColorX = (lineNoiseX - 0.5).xxx * 0.6; // 強調！

    //----------------------------------
    float scan = sin(uv.y * 600.0 + iTime * 300.0) * 0.1;

    //----------------------------------
    float flicker = (frac(sin(iTime * 12.345) * 43758.5453) - 0.5) * 0.15;

    //----------------------------------
    float luminance = dot(sceneColor.rgb, float3(0.3, 0.59, 0.11));
    float noiseWeight = saturate(1.7 - luminance * 1.5); // 強調！

    //----------------------------------
    color += (noiseColorY + noiseColorX) * noiseWeight;
    color += flicker;
    color -= scan;

    color = max(color, 0.08);

    color = pow(color, 1.05);

    return float4(saturate(color), 1.0);
}
