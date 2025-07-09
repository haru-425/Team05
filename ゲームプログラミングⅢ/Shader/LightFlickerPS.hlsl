#include "FullScreenQuad.hlsli"

Texture2D sceneTex : register(t10);
SamplerState samp : register(s0);

cbuffer FlickerParams : register(b13)
{
    float flickerSpeed; // ��������̃m�C�Y���x�i��F40.0�j
    float flickerStrength; // ���邳�̗h�炬�̋����i��F0.4�j
    float flickerDuration; // �������Ԃ��������ԁi�b�j�i��F1.0�j
    float flickerChance; // ��������N����m���i��F0.05�j
};


// �[�������_��
float rand(float x)
{
    return frac(sin(x * 12.9898) * 43758.5453);
}

// �m�C�Y�ɂ�邿����i�����g�̖��邳��炬�j
float getFlickerNoise(float time, float speed)
{
    float base = floor(time * speed);
    float t = frac(time * speed);
    float a = rand(base);
    float b = rand(base + 1.0);
    return lerp(a, b, t);
}

// "�������Ԃɓ����Ă��邩�ǂ���" �𔻒�
bool isInFlickerState(float time, float duration, float chance)
{
    float groupTime = floor(time / duration); // ��������u���b�N
    float r = rand(groupTime);
    return r < chance;
}

float4 main(VS_OUT input) : SV_Target
{
    float2 uv = input.texcoord;
    float4 color = sceneTex.Sample(samp, uv);

    float brightness = 1.0;

    // ���m���ł������ԂɂȂ�
    if (isInFlickerState(iTime, flickerDuration, flickerChance))
    {
        float noise = getFlickerNoise(iTime, flickerSpeed);
        brightness = 1.0 - (noise * flickerStrength);
    }

    color.rgb *= brightness;
    return color;
}
