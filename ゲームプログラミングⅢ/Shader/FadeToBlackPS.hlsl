#include "FullScreenQuad.hlsli"

#define LINEAR 1

// ���̓e�N�X�`��
Texture2D texture_map : register(t10);
SamplerState sampler_states[3] : register(s0);

// �萔�o�b�t�@�F�t�F�[�h���x
cbuffer FadeCBuffer : register(b0)
{
    float FadeAmount; // 0.0 = �\��, 1.0 = �^����
}

float4 main(VS_OUT pin) : SV_Target
{
    float4 color = texture_map.Sample(sampler_states[LINEAR], pin.texcoord);
    float FadeDuration = 1.0;
    float fade = saturate(signalTime / FadeDuration);
    // ���Ƃ̐��`���
    color.rgb = lerp(color.rgb, float3(0, 0, 0), fade);

    return color;
}
