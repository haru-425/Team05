  #include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);
float4 main(VS_OUT pin) : SV_TARGET
{
    return texture_map.Sample(sampler_states[LINEAR], pin.texcoord);
}

