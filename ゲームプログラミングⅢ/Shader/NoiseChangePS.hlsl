// �t���X�N���[���N���b�h�p�̓���
#include "FullScreenQuad.hlsli"


Texture2D sceneTex : register(t10);
SamplerState samp : register(s0);

float LineNoise(float y)
{
    // ��ʂ̍��������� N �{�̐������C���Ɋۂ߂�
    float Nline = floor(y * 300.0); // ���C���C���f�b�N�X
    return frac(sin(Nline + iTime * 20.0) * 43758.5453);
}

// ���C���s�N�Z���V�F�[�_�[
float4 main(VS_OUT pin) : SV_Target
{
    float TriggerInterval = 5.0; // �m�C�Y�����Ԋu�i�b�j
    float TransitionDuration = 0.5; // �t�F�[�h�C���E�A�E�g���ԁi�b�j

    float2 uv = pin.texcoord;
    float cycleTime = fmod(iTime, TriggerInterval);

    float strength = 0.0f;
    if (cycleTime < TransitionDuration)
    {
        float t = cycleTime / TransitionDuration;
        strength = sin(t * 3.14159); // ���炩�� 0 �� 1 �� 0
    }

    // �x�[�X�J���[
    float4 baseColor = sceneTex.Sample(samp, uv);

    // �����m�C�Y����
    float noise = LineNoise(uv.y);
    float4 noiseColor = float4(noise, noise, noise, 1.0);

    // �m�C�Y���t�F�[�h�ō���
    return lerp(baseColor, noiseColor, strength);
}
