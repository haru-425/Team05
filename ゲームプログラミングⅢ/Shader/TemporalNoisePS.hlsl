#include "FullScreenQuad.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}


float4 main(VS_OUT pin) : SV_Target
{

    float time = fmod(iTime, 1000.0); // ���Ԃ����[�v������
    float noise = rand(pin.texcoord * 100 + time * 20.0); // ���Ԃ�UV�Ɋ�Â��m�C�Y
    //float frameSeed = floor(time * 60.0); // �t���[���P�ʂ̃V�[�h�i60FPS�z��j

    // �t���[�����Ƃ̃����_���l�𐶐�
    //float flickerNoise = rand(float2(frameSeed, pin.texcoord.y * 100.0));


    float flicker = lerp(1.0, 1.5, noise); // �P�x�̗h�炬
    //float flicker = lerp(0.8, 1.2, flickerNoise); // �P�x�̗h�炬

    float4 color = texture_map.Sample(sampler_states[0], pin.texcoord);
    color.rgb *= flicker; // �P�x�Ƀm�C�Y��������
    color.a = 1.0; // �A���t�@�l��1.0�ɐݒ�i���߂��Ȃ��悤�ɂ���j
    return color;


}