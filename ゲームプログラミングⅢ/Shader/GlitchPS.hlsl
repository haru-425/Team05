#include "FullScreenQuad.hlsli"

#define CA_AMT 1.01
#define POINT 0
#define LINEAR 1

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

/**
 * @brief 289�Ŋ������]���Ԃ��֐�
 *
 * �l��289�Ń��b�v���邱�ƂŁA�p�[�~���e�[�V�����e�[�u���͈͓̔��Ɏ��߂邽�߂Ɏg�p���܂��B
 *
 * @param x ���͒l
 * @return float 289�Ń��b�v���ꂽ�l
 */
float mod289(float x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

/**
 * @brief 289�Ŋ������]���Ԃ��֐��ifloat2�o�[�W�����j
 *
 * 2�����x�N�g���̊e������289�Ń��b�v���܂��B
 *
 * @param x ���͒l�ifloat2�j
 * @return float2 289�Ń��b�v���ꂽ�l
 */
float2 mod289(float2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

/**
 * @brief 289�Ŋ������]���Ԃ��֐��ifloat3�o�[�W�����j
 *
 * 3�����x�N�g���̊e������289�Ń��b�v���܂��B
 *
 * @param x ���͒l�ifloat3�j
 * @return float3 289�Ń��b�v���ꂽ�l
 */
float3 mod289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

/**
 * @brief �l���V���b�t�����邽�߂̃p�[�~���[�g�֐�
 *
 * �m�C�Y�������̗����C���f�b�N�X����邽�߂Ɏg�p���܂��B
 *
 * @param x ���͒l�ifloat3�j
 * @return float3 �p�[�~���[�g���ꂽ�l
 */
float3 permute(float3 x)
{
    return mod289(((x * 34.0) + 1.0) * x);
}

/**
 * @brief 2�����̃V���v���b�N�X�m�C�Y�𐶐�����֐�
 *
 * �O���t�B�b�N�G�t�F�N�g�Ŏ��R�ȗh�炬�⃉���_������\�����邽�߂Ɏg���܂��B
 * Ian McEwan���̎�����HLSL�ɈڐA�������̂ł��B
 *
 * @param v ���͍��W�ifloat2�j
 * @return float �V���v���b�N�X�m�C�Y�l
 */
float snoise(float2 v)
{
    float4 C = float4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);

    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);

    float2 i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);

    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    i = mod289(i);
    float3 p = permute(permute(i.y + float3(0.0, i1.y, 1.0)) + i.x + float3(0.0, i1.x, 1.0));

    float3 m = max(0.5 - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m *= m * m * m;

    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

/**
 * @brief �����𐶐�����֐�
 *
 * ���͍��W�����l�����𐶐����A�m�C�Y��G�t�F�N�g�̃o���G�[�V�����ɗ��p���܂��B
 *
 * @param co ���͍��W�ifloat2�j
 * @return float �����l�i0�`1�j
 */
float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

/**
 * @brief �O���b�`���ʂ�t�^����s�N�Z���V�F�[�_�̃��C���֐�
 *
 * �m�C�Y��F�����A�����_���ȐF�ω���g�ݍ��킹�āA
 * �f���ɃO���b�`�i����j���ʂ�^���܂��B
 *
 * @param pin ���_�V�F�[�_����n�����o�͍\���́B�e�N�X�`�����W(texcoord)�Ȃǂ��܂݂܂��B
 * @return float4 �ŏI�I�ȐF�B�A���t�@�l�͏��1.0�ł��B
 */
float4 main(VS_OUT pin) : SV_Target
{
    float2 uv = pin.texcoord;
    float time = iTime * 2.0;
    float NoiseStrength = 0.2; // �h�炬�̋����i��F0.0 �` 1.0�j
    float NoiseScale = 6.0; // �m�C�Y�̃X�P�[���i�ׂ����j

    // �g���F�m�C�Y�ɃX�P�[���Ƌ��x�𔽉f
    float baseNoise = max(0.0, snoise(float2(time, uv.y * 0.3 * NoiseScale)) - 0.3) * (1.0 / 0.7);
    baseNoise += (snoise(float2(time * 10.0, uv.y * 2.4 * NoiseScale)) - 0.5) * 0.15;
    float noise = baseNoise * NoiseStrength;

    float xpos = uv.x - noise * noise * 0.01;
    float4 color = texture_map.Sample(sampler_states[LINEAR], float2(xpos, uv.y));
    color.a = 1;

    float r = rand(uv.y * time);
    float3 redTint = float3(r, r, r);
    color.rgb = lerp(color.rgb, redTint, noise * 0.3);

    float g = lerp(color.r, texture_map.Sample(sampler_states[LINEAR], float2(xpos + noise * 0.05, uv.y)).g, 1);
    float b = lerp(color.r, texture_map.Sample(sampler_states[LINEAR], float2(xpos - noise * 0.05, uv.y)).b, 1);
    color.g = g;
    color.b = b;

    return color;
}
