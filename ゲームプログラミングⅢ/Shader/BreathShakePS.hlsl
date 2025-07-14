#include "FullScreenQuad.hlsli"
#define CA_AMT 1.01
#define POINT 0
#define LINEAR 1
#define PI 3.14159265358979323846
SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

/**
 * @brief �ċz��S���̓�����͕킵����ʗh��E�Y�[�����ʂ�t�^����s�N�Z���V�F�[�_
 *
 * ���̃V�F�[�_�́A�ċz�̎����I�ȓ����ƐS���̏u�ԓI�ȗh���g�ݍ��킹�āA
 * ��ʑS�̂Ɏ��R�ȗh���Y�[�����ʂ�^���܂��B
 * �ċz�͔�Ώ̂Ȕg�`�ŕ\�����A�z�C�͂��₩�A�ċC�͋}���ȓ����ƂȂ�܂��B
 * �S���͎����I�ȃp���X�Ƃ��āA��ʂɏ����ȗh��������܂��B
 *
 * @param pin ���_�V�F�[�_����n�����o�͍\���́B�e�N�X�`�����W(texcoord)�Ȃǂ��܂݂܂��B
 * @return float4 �ŏI�I�ȐF�B�A���t�@�l�͏��1.0�ł��B
 */
float4 main(VS_OUT pin) : SV_TARGET
{
    // �O���[�o���ϐ�iTime����o�ߎ��ԁi�b�j���擾
    float time = iTime;

    // �s�N�Z���̃e�N�X�`�����W�i0�`1�͈̔́j
    float2 uv = pin.texcoord;

    // --- �ċz�p�����[�^�ݒ� ---
    float breathCycle = 4.0; // �ċz��1�����i�b�j
    float verticalAmplitude = 0.003; // �ċz�ɂ��㉺�h��̐U��
    float zoomAmplitude = 0.01; // �ċz�ɂ��Y�[���i�g��k���j�̐U��

    // �ċz�g�`�̐����i��Ώ́F�z�C�͂��₩�A�ċC�͋}���j
    float t = frac(time / breathCycle); // ���݂̌ċz�T�C�N�����̈ʒu�i0�`1�j
    float asymBreath;
    if (t < 0.6)
    {
        // �z�C�F0�`0.6�̊Ԃ�sin�J�[�u�ł��₩�ɏ㏸
        asymBreath = sin(t / 0.6 * PI * 0.5);
    }
    else
    {
        // �ċC�F0.6�`1.0�̊Ԃ͋}���ɉ��~
        asymBreath = -pow((t - 0.6) / 0.4, 0.5);
    }

    // �ċz�ɂ��㉺�h��iY�����I�t�Z�b�g�j
    float offsetY = asymBreath * verticalAmplitude;

    // �ċz�ɂ��Y�[���i�g��k���j
    float zoom = 1.0 + asymBreath * zoomAmplitude;
    float2 center = float2(0.5, 0.5); // ��ʒ��S���W
    // ��ʒ��S����Ɋg��k��
    uv = center + (uv - center) / zoom;

    // --- �S���p�����[�^�ݒ� ---
    float heartRate = 75.0 / 60.0; // �S�����iBPM��Hz�j
    // �S���̔g�`�isin�g�Ŏ����I�ɔ����j
    float pulse = sin(time * 2 * PI * heartRate);
    // �����̂Ƃ������L���i���̒l��0�ɂ���j
    pulse = max(pulse, 0.0);
    // �V���[�v�ŏ����ȗh��i�S���̏u�ԓI�ȓ����j
    pulse = pulse * pulse * 0.001;
    // �h��̕��������Ԃŉ�]�����邱�ƂŁA��莩�R�ȓ�����
    float2 pulseOffset = float2(pulse * cos(time * 10.0), pulse * sin(time * 10.0));

    // �ċz�ƐS���ɂ��UV���W�̍ŏI�␳
    uv += float2(0.0, offsetY) + pulseOffset;

    // �e�N�X�`������F���T���v�����O
    float4 color = texture_map.Sample(sampler_states[POINT], uv);

    // �A���t�@�l��1.0�ɐݒ�i��ɕs�����j
    color.a = 1.0;

    // �ŏI�I�ȐF��Ԃ�
    return color;
}