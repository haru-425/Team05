// BLOOM
#define POINT 0
#define LINEAR 1
SamplerState sampler_states[5] : register(s0);

Texture2D hdr_color_buffer_texture : register(t0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return hdr_color_buffer_texture.Sample(sampler_states[LINEAR], texcoord, 0.0);
}
