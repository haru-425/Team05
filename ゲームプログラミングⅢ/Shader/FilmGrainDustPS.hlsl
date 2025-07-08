#include "FullScreenQuad.hlsli"

#define LINEAR 1

Texture2D inputTex : register(t10);
SamplerState sampler_states[3] : register(s0);


// �^�������_������
float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// �O���C���m�C�Y
float GrainNoise(float2 uv, float time)
{
    float2 noiseUV = uv * 640.0 + time * 10.0;
    return rand(noiseUV);
}

// �z�R���m�C�Y�i�傫�߁j
float DustNoise(float2 uv, float time)
{
    float dust = 0.0;
    for (int i = 0; i < 3; ++i)
    {
        float scale = pow(1.8, i);
        float2 move = float2(sin(time * 0.1 + i * 5.0), cos(time * 0.15 + i * 8.0)) * 0.2;
        float2 dustUV = frac(uv * scale + move);
        float d = rand(dustUV);
        d = smoothstep(0.85, 1.0, d);
        dust += d * 0.5;
    }
    return saturate(dust);
}

// �X�L�������C���i���P�Łj
// - ���������Ԋu�ŏo��
// - ���Ԃɂ�艺�����ɃX�N���[��
float Scanline(float2 uv, float time)
{
    float speed = 0.1; // �X�N���[�����x
    float lineSpacing = 0.012; // ���C���̊Ԋu�i1.0 = ��ʑS�́j

    // UV.y�Ƀ^�C���I�t�Z�b�g�������ă��C�������炷
    float movingY = frac(uv.y + time * speed);

    // movingY��lineSpacing�̔{���ɋ߂��ꏊ�Ń��C�����o��
    float distanceToLine = fmod(movingY, lineSpacing);
    float mline = smoothstep(0.0, 0.0015, 0.001 - distanceToLine);

    return mline * 0.2; // ���߂Ɂi������������Βl���グ��j
}

// ���C���s�N�Z���V�F�[�_�[
float4 main(VS_OUT input) : SV_TARGET
{
    float2 uv = input.texcoord;

    float4 color = inputTex.Sample(sampler_states[LINEAR], uv);

    float grain = GrainNoise(uv, iTime) * 0.05;
    float dust = DustNoise(uv, iTime) * 0.6;
    float scan = Scanline(uv, iTime); // ���P�ς݃X�L�������C��

    // ����
    color.rgb = lerp(color.rgb, 1.0.xxx, dust * 0.35); // �z�R��
    color.rgb += grain; // �O���C��
    color.rgb -= scan; // �X�L�������C���i�Â����j

    return saturate(color);
}

