#include "FullScreenQuad.hlsli"

Texture2D sceneTex : register(t10);
SamplerState samp : register(s0);

cbuffer FlickerParams : register(b13)
{
    float flickerSpeed; // �m�C�Y�x�[�X�̂�������x�i��: 20.0�j
    float flickerStrength; // �m�C�Y�ɂ�閾�邳�̐U�ꕝ�i��: 0.4�j
    float flashInterval; // �傫�ȈÓ]�̎����i��: 4.0�b�j
    float flashDuration; // �Ó]���ԁi��: 0.2�b�j
};


// �[�������_���֐�
float rand(float x)
{
    return frac(sin(x * 12.9898) * 43758.5453);
}

// �p�[�����m�C�Y��������i�������Łj
float getFlickerNoise(float time, float speed)
{
    float base = floor(time * speed);
    float fracPart = frac(time * speed);
    float n0 = rand(base);
    float n1 = rand(base + 1.0);
    return lerp(n0, n1, fracPart);
}

// �u�����̂悤�ȑ傫�ȈÓ]����
float getFlash(float time, float interval, float duration)
{
    float cycleTime = fmod(time, interval);
    return (cycleTime < duration) ? 0.0 : 1.0;
}


float4 main(VS_OUT input) : SV_Target
{
    float2 uv = input.texcoord;
    float4 color = sceneTex.Sample(samp, uv);

    // �����g�m�C�Y�Ŗ��邳���ׂ����h�炷
    float flicker = getFlickerNoise(iTime, flickerSpeed);
    float brightness = 1.0 - (flicker * flickerStrength);

    // �������Ŋ��S�ɈÓ]����t�F�[�Y������
    float flash = getFlash(iTime, flashInterval, flashDuration);

    // �t���b�V���ɂ�鋭�������ƃm�C�Y������
    brightness *= flash;

    color.rgb *= brightness;
    return color;
}