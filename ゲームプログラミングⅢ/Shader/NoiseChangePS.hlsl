// �t���X�N���[���N���b�h�p�̓���
#include "FullScreenQuad.hlsli"

Texture2D sceneTex : register(t10);
SamplerState samp : register(s0);

float LineNoise(float y)
{
    float Nline = floor(y * 300.0);
    return frac(sin(Nline + iTime * 20.0) * 43758.5453);
}

float4 main(VS_OUT pin) : SV_Target
{
    float TriggerInterval = 5.0;
    float TransitionDuration = 0.5;

    float2 uv = pin.texcoord;
    float4 baseColor = sceneTex.Sample(samp, uv);

    if (iTime < TriggerInterval)
    {
        return baseColor;
    }

    float cycleTime = fmod(iTime - TriggerInterval, TriggerInterval);
    float strength = 0.0f;

    if (cycleTime < TransitionDuration)
    {
        float t = cycleTime / TransitionDuration;
        strength = sin(t * 3.14159); // ���炩�ȃt�F�[�h
    }

    // �m�C�Y����
    float noise = LineNoise(uv.y);

    // �Â��w�i�ւ̓���݂��l�������m�C�Y�J���[�␳
    float3 tint = float3(0.1, 0.05, 0.0); // �g�F���̃x�[�X�g�[��
    float3 noiseColor = lerp(tint, float3(noise, noise, noise), 0.7);

    // �w�i�̋P�x�����ɍ������x�𒲐�
    float luminance = dot(baseColor.rgb, float3(0.299, 0.587, 0.114));
    float contrastFactor = saturate(1.0 - luminance);

    float blend = strength * contrastFactor;

    // �����F�Â��w�i�ɂ���a���Ȃ��m�C�Y���n������
    float3 finalColor = lerp(baseColor.rgb, noiseColor, blend);

    return float4(finalColor, 1.0);
}