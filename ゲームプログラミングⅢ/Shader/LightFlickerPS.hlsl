#include "FullScreenQuad.hlsli"

Texture2D sceneTex : register(t10); ///< �V�[���̃e�N�X�`��
SamplerState samp : register(s0); ///< �T���v���[�X�e�[�g

/**
 * @brief ������p�����[�^�p�萔�o�b�t�@
 */
cbuffer FlickerParams : register(b13)
{
    float flickerSpeed; ///< ��������̃m�C�Y���x�i��F40.0�j
    float flickerStrength; ///< ���邳�̗h�炬�̋����i��F0.4�j
    float flickerDuration; ///< �������Ԃ��������ԁi�b�j�i��F1.0�j
    float flickerChance; ///< ��������N����m���i��F0.05�j
}

/**
 * @brief �[�������_���֐�
 * @param x ���͒l�i���Ԃ�V�[�h�Ȃǁj
 * @return 0�`1�̋^�������l
 */
float rand(float x)
{
    return frac(sin(x * 12.9898) * 43758.5453);
}

/**
 * @brief ���`��ԕt�����炩�ȃm�C�Y�����֐�
 * @param time ���ݎ���
 * @param speed �m�C�Y�ω����x
 * @return 0�`1�̊��炩�ȃm�C�Y�l
 */
float smoothNoise(float time, float speed)
{
    float base = floor(time * speed);
    float t = frac(time * speed);
    float a = rand(base);
    float b = rand(base + 1.0);
    return lerp(a, b, t);
}

/**
 * @brief �������Ԃ��ǂ����𔻒�
 * @param time ���ݎ���
 * @param duration ������������ԁi�b�j
 * @param chance ����������m���i0�`1�j
 * @return true�Ȃ猻�݂�������
 */
bool isFlickering(float time, float duration, float chance)
{
    float group = floor(time / duration);
    return rand(group) < chance;
}

/**
 * @brief ������p�^�[���𗐐��őI���i0�`15�j
 * @param seed �V�[�h�l�i��F������O���[�v���ԂȂǁj
 * @return �p�^�[��ID�i0�`15�j
 */
int choosePattern(float seed)
{
    return (int) (rand(seed) * 16.0);
}

/**
 * @brief �t�F�[�h�C���E�A�E�g�p�̎O�p�g�v�Z
 * @param t 0�`1�̎��ԃp�����[�^
 * @return 0�`1�̃t�F�[�h�l�i�O�p�g�j
 */
float fadeInOut(float t)
{
    return 1.0 - abs(frac(t) * 2.0 - 1.0);
}

/**
 * @brief �p�^�[�����Ƃ̖��邳�i�P�x�j�v�Z
 * @param time ���ݎ���
 * @param pattern �p�^�[��ID�i0�`15�j
 * @param flickerSpeed �m�C�Y���x�p�����[�^
 * @param flickerStrength �h�炬���x�p�����[�^
 * @return 0�`1�̖��x�l
 */
float getBrightness(float time, int pattern, float flickerSpeed, float flickerStrength)
{
    switch (pattern)
    {
        case 0:
            // �����m�C�Y�ɂ����ׂȖ��x�h�炬
            return 1.0 - smoothNoise(time, flickerSpeed) * flickerStrength;

        case 1:
            // ��u���S�����i�u�ԓI�ɐ^���ÂɂȂ�j
            return fmod(time * 10.0, 1.0) < 0.1 ? 0.0 : 1.0;

        case 2:
        {
            // �A���_�Łi���߂̈ÂƖ����J��Ԃ��j
                float t = fmod(time * 5.0, 1.0);
                return (t < 0.3 || (t > 0.5 && t < 0.6)) ? 0.3 : 1.0;
            }

        case 3:
            // �����_���ɖ��x���W�����v����s�K�������
            return lerp(1.0, 0.5, rand(floor(time * 20.0) * 17.0));

        case 4:
        {
            // �������t�F�[�h�C���E�A�E�g���鉸�₩�ȗh�炬
                float t = frac(time * 1.5);
                return lerp(0.7, 1.0, abs(sin(t * 3.14159)));
            }

        case 5:
        {
            // �����̑����t���b�V�����d�˂����G�Ȃ����
                float basePulse = smoothNoise(time * 20.0, 40.0);
                float flickerPulse = smoothNoise(time * 60.0, 80.0);
                float flicker = lerp(basePulse, flickerPulse, 0.5);
                return 1.0 - flicker * flickerStrength * 1.5;
            }

        case 6:
        {
            // �s�K���ȓ_�Łi���ԂɃm�C�Y�������ă��Y��������j
                float noiseTime = time + rand(floor(time * 3.0) * 7.0);
                float t = fmod(noiseTime * 6.0, 1.0);
                return (t < 0.25) ? 0.0 : 1.0;
            }

        case 7:
        {
            // �ɂ₩�ȗh�炬�ɏu�ԓI�ȈÓ]���d�˂������p�^�[��
                float base = lerp(0.8, 1.0, smoothNoise(time, 2.0));
                float flash = fmod(time * 12.0, 1.0) < 0.05 ? 0.0 : 1.0;
                return base * flash;
            }

        case 8:
        {
            // ���X�ɖ��邭�Ȃ�A�}�ɈÓ]���邶����_���p�^�[��
                float t = frac(time * 2.0);
                return (t < 0.8) ? lerp(0.5, 1.0, t / 0.8) : 0.0;
            }

        case 9:
        {
            // �Z���Ԋu�̍����_�Łi�p�`�p�`�Ƃ������Â��J��Ԃ��j
                float t = fmod(time * 30.0, 1.0);
                return t < 0.15 ? 0.0 : 1.0;
            }

        case 10:
        {
            // ���X�ɖ��邳�������Ĉ�u�Ó]�A�J��Ԃ����߂̂����
                float t = frac(time * 1.2);
                if (t < 0.9)
                    return lerp(1.0, 0.2, t / 0.9);
                else
                    return 0.0;
            }

        case 11:
        {
            // �m�C�Y�Ǝ����_�ł�g�ݍ��킹���s����Ȃ����
                float base = 1.0 - smoothNoise(time, flickerSpeed) * flickerStrength * 0.6;
                float pulse = (fmod(time * 7.0, 1.0) < 0.4) ? 0.6 : 1.0;
                return base * pulse;
            }

        case 12:
        {
            // ����g��炬�{�����_����u�Ó]��g�ݍ��킹���p�^�[��
                float slowNoise = lerp(0.85, 1.0, smoothNoise(time, 0.5));
                float flash = rand(floor(time * 4.0) * 19.0) < 0.1 ? 0.0 : 1.0;
                return slowNoise * flash;
            }

        case 13:
        {
            // �}���ȓ_�łƂ�邢�t�F�[�h�̍����p�^�[��
                float pulse = fmod(time * 15.0, 1.0);
                float fade = frac(time * 2.0);
                float base = (pulse < 0.1) ? 0.0 : lerp(0.7, 1.0, fade);
                return base;
            }

        case 14:
        {
            // �ˑR�̈Ó]���s�K���ɔ�������p�^�[��
                float noise = rand(floor(time * 10.0) * 13.0);
                return noise < 0.15 ? 0.0 : 1.0;
            }

        case 15:
        {
            // �ɂ₩�Ȗ��x�㏸�Ɠ˔��I�ȏ����ȃt���b�V�����������p�^�[��
                float base = lerp(0.6, 1.0, frac(time * 0.8));
                float flash = fmod(time * 40.0, 1.0) < 0.03 ? 1.2 : 1.0;
                return saturate(base * flash);
            }

        default:
            // ���S��Ƃ��Ēʏ햾�邳
            return 1.0;
    }
}

/**
 * @brief �s�N�Z���V�F�[�_�[���C���֐�
 * @param input ���_�V�F�[�_�[����̓��́iUV���W�܂ށj
 * @return ��������������ŏI�F
 */
float4 main(VS_OUT input) : SV_Target
{
    float2 uv = input.texcoord;

    // ���V�[���e�N�X�`���F�擾
    float4 color = sceneTex.Sample(samp, uv);

    float brightness = 1.0;

    // ���݂̂�����O���[�v���ԁi������������ԒP�ʂŋ�؂�j
    float groupTime = floor(iTime / flickerDuration);

    // �������ԂȂ�p�^�[���I�����Ė��x�v�Z
    if (isFlickering(iTime, flickerDuration, flickerChance))
    {
        int pattern = choosePattern(groupTime);
        float flickerTime = frac(iTime / flickerDuration);
        float fade = fadeInOut(flickerTime);

        float baseBrightness = getBrightness(iTime, pattern, flickerSpeed, flickerStrength);

        // �t�F�[�h�C���A�E�g�Ŗ��x�����炩�ɕω�������
        brightness = lerp(1.0, baseBrightness, fade);
    }

    // �ŏI�I�ɃV�[���F�ɖ��x����Z
    color.rgb *= brightness;

    return color;
}



//#include "FullScreenQuad.hlsli"
//
//Texture2D sceneTex : register(t10);
//SamplerState samp : register(s0);
//
//cbuffer FlickerParams : register(b13)
//{
//    float flickerSpeed; // ��������̃m�C�Y���x�i��F40.0�j
//    float flickerStrength; // ���邳�̗h�炬�̋����i��F0.4�j
//    float flickerDuration; // �������Ԃ��������ԁi�b�j�i��F1.0�j
//    float flickerChance; // ��������N����m���i��F0.05�j
//};
//
//
//// �[�������_��
//float rand(float x)
//{
//    return frac(sin(x * 12.9898) * 43758.5453);
//}
//
//// �m�C�Y�ɂ�邿����i�����g�̖��邳��炬�j
//float getFlickerNoise(float time, float speed)
//{
//    float base = floor(time * speed);
//    float t = frac(time * speed);
//    float a = rand(base);
//    float b = rand(base + 1.0);
//    return lerp(a, b, t);
//}
//
//// "�������Ԃɓ����Ă��邩�ǂ���" �𔻒�
//bool isInFlickerState(float time, float duration, float chance)
//{
//    float groupTime = floor(time / duration); // ��������u���b�N
//    float r = rand(groupTime);
//    return r < chance;
//}
//
//float4 main(VS_OUT input) : SV_Target
//{
//    float2 uv = input.texcoord;
//    float4 color = sceneTex.Sample(samp, uv);
//
//    float brightness = 1.0;
//
//    // ���m���ł������ԂɂȂ�
//    if (isInFlickerState(iTime, flickerDuration, flickerChance))
//    {
//        float noise = getFlickerNoise(iTime, flickerSpeed);
//        brightness = 1.0 - (noise * flickerStrength);
//    }
//
//    color.rgb *= brightness;
//    return color;
//}
