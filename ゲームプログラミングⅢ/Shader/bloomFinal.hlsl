// BLOOM
#include "fullscreenquad.hlsli"

#define POINT 0
#define LINEAR 1
SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[2] : register(t10);

float3 reinhard_tone_mapping(float3 color)
{
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float tone_mapped_luma = luma / (1. + luma);
    color *= tone_mapped_luma / luma;
    return color;
}
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_maps[0].Sample(sampler_states[POINT], pin.texcoord);
    color.a = 1;
    float4 bloom = texture_maps[1].Sample(sampler_states[POINT], pin.texcoord);
    color.a = 1.0; // Ensure alpha is set to 1 for the final output
    float3 fragment_color = color.rgb + bloom.rgb;
    float alpha = color.a;

	// Tone map
        fragment_color = reinhard_tone_mapping(fragment_color);

	// Gamma correction
    const float INV_GAMMA = 1.0 / 2.2;
    fragment_color = pow(fragment_color, INV_GAMMA);

    return float4(fragment_color, 1);
}
