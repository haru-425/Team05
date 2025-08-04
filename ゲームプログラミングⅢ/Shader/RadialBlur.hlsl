#include "FullScreenQuad.hlsli"
// ���̓e�N�X�`���ƃT���v��
Texture2D InputTexture : register(t0);
SamplerState InputSampler : register(s0);

// �p�����[�^
cbuffer RadialBlurParams : register(b0)
{
    float2 Center; // ���S���W�iUV��ԁA��Ffloat2(0.5, 0.5)�j
    float BlurStrength; // �u���[�̋����i��F0.2�j
    int SampleCount; // �T���v�����i��F16�j
    float FalloffPower; // �t�H�[���I�t�w���i��F2.0�j
    float2 DirectionBias; // �����o�C�A�X�i��Ffloat2(1.0, 0.5)�j
};

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 dir = (pin.texcoord - Center) * DirectionBias;
    float2 step = dir * (BlurStrength / SampleCount);

    float4 accumulatedColor = float4(0, 0, 0, 0);
    float totalWeight = 0.0;

    for (int i = 0; i < SampleCount; ++i)
    {
        float t = (float) i / (SampleCount - 1);
        float weight = pow(t, FalloffPower); // �O���قǏd��

        float2 sampleUV = pin.texcoord - step * i;
        accumulatedColor += InputTexture.Sample(InputSampler, sampleUV) * weight;
        totalWeight += weight;
    }

    return accumulatedColor / totalWeight;
}
