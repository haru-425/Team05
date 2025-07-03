#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define SHARPEN_FACTOR 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

// �J�n���ԁi�b�j�ƌp�����ԁi�b�j��ݒ�
static const float startTime = 1.0; // ��: 1�b��ɊJ�n
static const float duration = 0.3; // ��: 0.3�b�����ďI��

float4 main(VS_OUT pin) : SV_Target
{
    float2 uv = pin.texcoord;
    float time = iTime; // �O���[�o���ϐ�iTime����o�ߎ��ԁi�b�j���擾

    // �G�t�F�N�g�i�s�x�i0.0�`1.0�j���v�Z
    float progress = saturate((time - startTime) / duration);

    // ��ʂ̒��S���擾
    float2 center = float2(0.5, 0.5);
    float2 delta = uv - center;

    // ���Ԃ��i�ނɂ�āA�����֎������Đ���ɂȂ��Ă���
    float collapse = pow(progress, 2.0); // �ɂ₩�ȊJ�n�A�����������
    delta.y /= (1.0 - collapse); // ������������
    delta.x *= (1.0 - collapse * 0.1); // �c�����͂킸���Ɏ���

    float2 displacedUV = center + delta;
    float4 color = texture_map.Sample(sampler_states[0], displacedUV);

    // --- �X�L�������C������ ---
    float scanline = 0.85 + 0.15 * sin(uv.y * 1080.0 * 3.14159); // 1080�͉𑜓x�ɉ����Ē���
    color.rgb *= scanline;

    // --- �z���C�g�A�E�g���ʁi�i�s�x0.9�ȍ~�Ŕ����j ---
    float whiteout = smoothstep(0.9, 1.0, progress);
    color.rgb = lerp(color.rgb, float3(1.0, 1.0, 1.0), whiteout);

    // --- ���S�I�����͍� ---
    if (progress >= 1.0)
    {
        color.rgb = float3(0.0, 0.0, 0.0);
    }

    return float4(color.rgb, 1);
}