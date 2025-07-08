#include "FullScreenQuad.hlsli"

/// @file WardenGazePS.hlsl
/// @brief �����̗h�炬��T�����ʂ�\������|�X�g�v���Z�X�p�s�N�Z���V�F�[�_�[
/// @details �^�������_���⎞�ԕω��ɂ�鎋���̓����A�e�N�X�`���T���v�����O���s���B
///          �T���v���[��e�N�X�`���A�e�탆�[�e�B���e�B�֐����`�B

//----------------------------------------
// �萔��`
//----------------------------------------

/// @brief �F�����̋��x�W��
#define CA_AMT 1.01

/// @brief �T���v���[�̎��: Point�T���v�����O
#define POINT 0
/// @brief �T���v���[�̎��: Linear�T���v�����O
#define LINEAR 1
/// @brief �T���v���[�̎��: Anisotropic�T���v�����O
#define ANISOTROPIC 2

//----------------------------------------
// �T���v���[�X�e�[�g�E�e�N�X�`��
//----------------------------------------

/// @brief �T���v���[�X�e�[�g�z��
/// @details POINT, LINEAR, ANISOTROPIC ��3��ނ̃T���v�����O�������i�[
SamplerState sampler_states[3] : register(s0);

/// @brief ���̓e�N�X�`��
Texture2D texture_map : register(t10);

//----------------------------------------
// �^�������_�������֐�
//----------------------------------------

/// @brief �^�������_���l�𐶐�����֐�
/// @param seed �����_���V�[�h�ƂȂ�l
/// @return 0.0�`1.0�͈̔͂̋^�������_���l
float rand(float seed)
{
    return frac(sin(seed * 12.9898) * 43758.5453);
}

/// @brief 2�����̋^�������_���l�𐶐�����֐�
/// @param seed �����_���V�[�h�ƂȂ�l
/// @return float2�^�̋^�������_���l�i�e����0.0�`1.0�j
float2 rand2(float seed)
{
    return float2(rand(seed), rand(seed + 42.0));
}

//----------------------------------------
// �����_���p�����[�^����
//----------------------------------------

/// @brief X���EY�����ꂼ��Ń����_���ȐU���E�����p�����[�^�𐶐�
/// @param[in]  t        ���݂̎���
/// @param[out] amplitude X�EY�����Ƃ̐U���i�h�ꕝ�j
/// @param[out] period    X�EY�����Ƃ̎����i�h��鑬���j
/// @details ���Ԍo�߂ɉ����Ċ��炩�Ƀp�����[�^���ω�����悤��Ԃ���
void GetRandomParams2(float t, out float2 amplitude, out float2 period)
{
    float interval = 2.0; ///< �p�����[�^���؂�ւ��Ԋu�i�b�j

    float currentIndex = floor(t / interval); ///< ���݂̃C���f�b�N�X
    float progress = frac(t / interval); ///< �C���f�b�N�X���ł̐i�s�x�i0.0�`1.0�j

    // X���p��Y���p�ňقȂ�V�[�h�l���g�p
    float2 prevX = rand2(currentIndex);
    float2 nextX = rand2(currentIndex + 1.0);
    float2 prevY = rand2(currentIndex + 100.0); // �V�[�h�����炷���ƂœƗ������m��
    float2 nextY = rand2(currentIndex + 101.0);

    // X���̐U���E�����i�O��̒l���ԁj
    float prevAmpX = lerp(0.04, 0.06, prevX.x);
    float nextAmpX = lerp(0.04, 0.06, nextX.x);
    float prevPeriodX = lerp(4.0, 4.0, prevX.y);
    float nextPeriodX = lerp(4.0, 4.0, nextX.y);

    // Y���̐U���E�����i�O��̒l���ԁj
    float prevAmpY = lerp(0.002, 0.01, prevY.x);
    float nextAmpY = lerp(0.002, 0.01, nextY.x);
    float prevPeriodY = lerp(3.0, 4.0, prevY.y);
    float nextPeriodY = lerp(3.0, 4.0, nextY.y);

    // �X���[�Y�ɒl���Ԃ��ďo��
    amplitude.x = lerp(prevAmpX, nextAmpX, smoothstep(0.0, 1.0, progress));
    amplitude.y = lerp(prevAmpY, nextAmpY, smoothstep(0.0, 1.0, progress));
    period.x = lerp(prevPeriodX, nextPeriodX, smoothstep(0.0, 1.0, progress));
    period.y = lerp(prevPeriodY, nextPeriodY, smoothstep(0.0, 1.0, progress));
}

//----------------------------------------
// �����̗h�炬�I�t�Z�b�g�v�Z
//----------------------------------------

/// @brief ���Ԃɉ����������̗h�炬�I�t�Z�b�g���v�Z
/// @param t ���݂̎���
/// @return float2�^�̃I�t�Z�b�g�l�iUV���W�ɉ��Z���Ďg�p�j
/// @details X���EY�����ƂɈقȂ�����E�U���ŗh�炬�𐶐�
float easeInOutElastic(float x)
{
    const float c5 = (2.0 * 3.14159) / 4.5;

    if (x < 0.5)
    {
        return -(pow(2.0, 20.0 * x - 10.0) * sin((20.0 * x - 11.125) * c5)) * 0.5;
    }
    else
    {
        return (pow(2.0, -20.0 * x + 10.0) * sin((20.0 * x - 11.125) * c5)) * 0.5 + 1.0;
    }
}

// PingPong�^�̐i�s�x�i0��1��0��1�c�j
float pingpong(float x)
{
    return 1.0 - abs(1.0 - frac(x) * 2.0);
}

float2 GetEyeOffset(float t)
{
    float2 amplitude, period;
    GetRandomParams2(t, amplitude, period);

    // �s���|���i�s�x�𐶐��i�ʑ����炵�܂ށj
    float progressX = pingpong(t / period.x);
    float progressY = pingpong(t / period.y + 0.3); // Y���͂��炵�Ď��R�����o��

    // Elastic�C�[�W���O��K�p�� [-1,1] �͈̔͂ɕϊ�
    float x = (easeInOutElastic(progressX) * 2.0 - 1.0) * amplitude.x;
    float y = (easeInOutElastic(progressY) * 2.0 - 1.0) * amplitude.y;

    return float2(x, y);
}
//----------------------------------------
// ���C���s�N�Z���V�F�[�_�[
//----------------------------------------

/// @brief �|�X�g�v���Z�X�p���C���s�N�Z���V�F�[�_�[
/// @param pin ���_�V�F�[�_�[����n�����\���́iUV���W�Ȃǂ��܂ށj
/// @return �o�̓J���[�ifloat4�j
/// @details ���Ԃɉ����Ď������h�炮�悤��UV���W��ω������A�e�N�X�`�����T���v�����O���ďo��
float4 main(VS_OUT pin) : SV_Target
{
    //--- �����̗h�炬�I�t�Z�b�g���v�Z ---
    float2 offset = GetEyeOffset(iTime);

    //--- UV���W�ɃI�t�Z�b�g�����Z ---
    float2 new_uv = pin.texcoord + offset;

    //--- �e�N�X�`�����T���v�����O ---
    float4 color = texture_map.Sample(sampler_states[LINEAR], new_uv);

    //--- �ŏI�J���[���o�� ---
    return color;
}
