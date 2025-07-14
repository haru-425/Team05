#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define SHARPEN_FACTOR 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

// --- ���ʏI���܂ł̎��ԁi�b�j��ݒ� ---
static const float effectEndTime = 2.0; // ������ύX����Ή��o�̒����𒲐��\

float4 main(VS_OUT pin) : SV_Target
{
    float time = iTime; // ���Ԍo�߁i�b�j
    float2 resolution = float2(1280, 720); // ��ʉ𑜓x
    float mode = 0; // 0.0: �N�� / 1.0: �V���b�g�_�E��

    float startup = 1.0 - mode; // �N����:1 �� �V���b�g�_�E����:0

    // --- ���ʐi�s�x�i0.0�`1.0�j---
    float progress = saturate(time / effectEndTime);
    float effectStrength = (mode == 0.0) ? (1.0 - progress) : progress;
    // �N����: 1��0, �V���b�g�_�E����: 0��1

    // �N���}�e�B�b�N�A�o���[�V����
    float offset = 0.003 * effectStrength;
    float3 color;
    color = texture_map.Sample(sampler_states[LINEAR], pin.texcoord).rgb;
   // color.r = texture_map.Sample(sampler_states[0], pin.texcoord - float2(offset, 0)).r;
   // color.g = texture_map.Sample(sampler_states[0], pin.texcoord).g;
   // color.b = texture_map.Sample(sampler_states[0], pin.texcoord + float2(offset, 0)).b;

    // ����/�g�U�t�H�[�J�X
    float2 center = float2(0.5, 0.5);
    float dist = distance(pin.texcoord, center);
    float blurBias = saturate(1.0 - time * 0.5);
    float blurAmount = lerp(blurBias, 0.0, mode);
    float focus = smoothstep(0.35, 0.0, dist) * (1.0 - blurAmount * effectStrength);

    float blurOffset = 0.002 * effectStrength;
    float3 blur =
    (
        texture_map.Sample(sampler_states[LINEAR], pin.texcoord + float2(blurOffset, 0)).rgb +
        texture_map.Sample(sampler_states[LINEAR], pin.texcoord - float2(blurOffset, 0)).rgb +
        texture_map.Sample(sampler_states[LINEAR], pin.texcoord + float2(0, blurOffset)).rgb +
        texture_map.Sample(sampler_states[LINEAR], pin.texcoord - float2(0, blurOffset)).rgb
    ) * 0.25;
    color = lerp(blur, color, focus);

    // �X�L�������C��
 //   float scanStrength = 0.05 * effectStrength;
 //   float scan = sin((pin.texcoord.y + time * (1.5 * (startup * 2.0 - 1.0))) * resolution.y * 0.05) * scanStrength;
 //   color += scan;

    // �t�F�[�h�C��/�A�E�g
    float fade = (mode == 0.0) ? progress : (1.0 - progress);
    color *= fade;

    // --- ���ʏI�����̂Ȃ߂炩�ȑJ�� ---
    float3 baseColor = texture_map.Sample(sampler_states[LINEAR], pin.texcoord).rgb;
    if (progress >= 1.0)
    {
        if (mode == 0.0)
        {
            // �ʏ�\��
            return float4(baseColor, 1.0);
        }
        else
        {
            // �V���b�g�_�E�����͍�
            return float4(0, 0, 0, 1);
        }
    }
    else
    {
        // ���ʂ����X�Ɏ�߂Ă���
        if (mode == 0.0)
        {
            // �N����: �G�t�F�N�g���ʏ�
            color = lerp(color, baseColor, progress);
            return float4(color, 1.0);
        }
        else
        {
            // �V���b�g�_�E����: �G�t�F�N�g����
            color = lerp(color, float3(0, 0, 0), progress);
            return float4(color, 1.0);
        }
    }
}