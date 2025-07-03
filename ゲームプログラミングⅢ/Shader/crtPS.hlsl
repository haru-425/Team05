#include "FullScreenQuad.hlsli"
#define CURVATURE 4.2
#define BLUR .021
//#define CA_AMT 1.024
#define CA_AMT 1.01
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

/**
 * @brief CRT�f�B�X�v���C���̘c�݁E�F�����E�������E�G�b�W�t�F�[�h���ʂ�t�^����s�N�Z���V�F�[�_
 *
 * ���̃V�F�[�_�́AUV���W�̘p�ȁA�G�b�W�̃t�F�[�h�A�E�g�A�F�����iRGB���炵�j�A
 * ��������}�X�N�ɂ��CRT�f�B�X�v���C���L�̕\�����������A
 * ���g���ȉf���\�����������܂��B
 *
 * @param pin ���_�V�F�[�_����n�����o�͍\���́B�e�N�X�`�����W(texcoord)��s�N�Z���ʒu(position)���܂݂܂��B
 * @return float4 �ŏI�I�ȐF�B�A���t�@�l�͏��1.0�ł��B
 */
float4 main(VS_OUT pin) : SV_Target
{
    // �e�N�X�`�����W�i0�`1�͈̔́j
    float2 uv = pin.texcoord;

    // --- CRT�p�Ȍ��� ---
    // UV���W��-1�`1�ɕϊ����AY��X�����ւ��Ęc�ݗʂ��v�Z
    float2 crtUV = uv * 2.0 - 1.0;
    float2 offset = crtUV.yx / CURVATURE;
    // �I�t�Z�b�g��2����|���Ĕ���`�Șp�Ȃ�������
    crtUV += crtUV * offset * offset;
    // ����UV��ԁi0�`1�j�ɖ߂�
    crtUV = crtUV * 0.5 + 0.5;

    // --- �G�b�W�t�F�[�h���� ---
    // ��ʒ[�ŏ��X�ɓ����ɂȂ�悤�ɃG�b�W�̋��x���v�Z
    float2 edge = smoothstep(0.0, BLUR, crtUV) * (1.0 - smoothstep(1.0 - BLUR, 1.0, crtUV));

    // --- �F�����i�N���}�`�b�N�A�o���[�V�����j ---
    // R,G,B�e�������킸���ɂ��炵�ăT���v�����O���A�F�Y����\��
    float3 col;
    // �Ԑ����͍������ɂ��炵�ăT���v�����O
    col.r = texture_map.Sample(sampler_states[2], (crtUV - 0.5) * CA_AMT + 0.5).r;
    // �ΐ����͂��̂܂܃T���v�����O
    col.g = texture_map.Sample(sampler_states[2], crtUV).g;
    // �����͉E�����ɂ��炵�ăT���v�����O
    col.b = texture_map.Sample(sampler_states[2], (crtUV - 0.5) / CA_AMT + 0.5).b;

    // �G�b�W�t�F�[�h��RGB�S�̂ɓK�p
    col *= edge.x * edge.y;

    // --- �������E�}�X�N���� ---
    // Y���W��2�s�N�Z�����Ƃɖ��邳�������đ�������\��
    // X���W��3�s�N�Z�����Ƃɖ��邳�������ă}�X�N���ʂ�ǉ�
    if (fmod(pin.position.y, 2.0) < 1.0)
        col *= 0.7;
    else if (fmod(pin.position.x, 3.0) < 1.0)
        col *= 0.7;
    else
        col *= 1.2;

    // �ŏI�I�ȐF��Ԃ��i�A���t�@�l�͏��1.0�j
    return float4(col, 1.0);
}