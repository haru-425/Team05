//#include "FullScreenQuad.hlsli"
//// ���̓e�N�X�`���ƃT���v��
//Texture2D InputTexture : register(t10);
//SamplerState InputSampler : register(s0);
//
//// �p�����[�^
//cbuffer RadialBlurParams : register(b9)
//{
//    float2 Center; // ���S���W�iUV��ԁA��Ffloat2(0.5, 0.5)�j
//    float BlurStrength; // �u���[�̋����i��F0.2�j
//    int SampleCount; // �T���v�����i��F16�j
//    float FalloffPower; // �t�H�[���I�t�w���i��F2.0�j
//    float2 DirectionBias; // �����o�C�A�X�i��Ffloat2(1.0, 0.5)�j
//    float padding; // �p�f�B���O
//};
//
//float4 main(VS_OUT pin) : SV_TARGET
//{
//    float2 dir = (pin.texcoord - Center) * DirectionBias;
//    float2 step = dir * (BlurStrength / SampleCount);
//
//    float4 accumulatedColor = float4(0, 0, 0, 0);
//    float totalWeight = 0.0;
//
//    for (int i = 0; i < SampleCount; ++i)
//    {
//        float t = (float) i / (SampleCount - 1);
//        float weight = pow(t, FalloffPower); // �O���قǏd��
//
//        float2 sampleUV = pin.texcoord - step * i;
//        accumulatedColor += InputTexture.Sample(InputSampler, sampleUV) * weight;
//        totalWeight += weight;
//    }
//
//    return accumulatedColor / totalWeight;
//}

#include "FullScreenQuad.hlsli"
Texture2D InputTexture : register(t10);
SamplerState InputSampler : register(s0);

cbuffer RadialBlurParams : register(b9)
{
    float2 Center; // �u���[���S�iUV�j
    float BlurStrength; // �u���[�̋���
    int SampleCount; // �T���v����
    float FalloffPower; // �t�H�[���I�t���x�i�w���j
    float2 DirectionBias; // �����o�C�A�X
    float OffsetStrength; // �T���v���J�n�_�𒆐S���炸�炷�ʁi0?1�j
    float CenterFeather; // ���S�ڂ����y���i0?1�j
    float BlendAmount; // ���摜�Ƃ̍�����i0?1�j
};

float4 main(VS_OUT pin) : SV_Target
{
    float2 uv = pin.texcoord;
    float2 dir = (uv - Center) * DirectionBias;
    float len = length(dir);

    // ���S�߂��ł̃u���[������
    float feather = 1.0 - saturate(len * CenterFeather);

    // �X�e�b�v�����v�Z
    float2 step = dir * (BlurStrength * feather / SampleCount);

    float2 offsetStart = step * OffsetStrength; // ���S����̏����I�t�Z�b�g

    float4 accumulatedColor = float4(0, 0, 0, 0);
    float totalWeight = 0.0;

    for (int i = 0; i < SampleCount; ++i)
    {
        float t = (float) i / (SampleCount - 1);
        float weight = pow(t, FalloffPower);

        float2 sampleUV = uv - step * i - offsetStart;
        accumulatedColor += InputTexture.Sample(InputSampler, sampleUV) * weight;
        totalWeight += weight;
    }

    float4 blurred = accumulatedColor / totalWeight;

    // ���摜�ƃu���[�̃u�����h
    float4 original = InputTexture.Sample(InputSampler, uv);
    return lerp(original, blurred, BlendAmount);
}