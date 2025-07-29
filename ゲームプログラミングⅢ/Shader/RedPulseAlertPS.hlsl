#include "fullscreenquad.hlsli"

SamplerState sampler_states : register(s0);
Texture2D texture_maps : register(t10);

float gIntensity = 1.0f; // 0.0�`1.0�F�ǐՂ���Ă��Ȃ��Ƃ��� 0.0
float gBorderWidth = 1.0f;

// ��: 0.1 = �O��10%�ɃG�t�F�N


float4 main(VS_OUT pin) : SV_Target
{
    //float2 uv = pin.texcoord;


   //// ���S����̋����i0�`1�j
   //float2 center = float2(0.5, 0.5);
   //float dist = distance(uv, center);
   //
   //// �G�b�W����̋����i0 = ���S�Ȓ��S, 1 = �p�j
   //float edgeFade = saturate((dist - (0.5 - gBorderWidth)) / gBorderWidth);
   //
   //// ���Ԃɂ��_�� (sin�g�� 0�`1 ��)
   //float blink = 0.5 + 0.5 * sin(iTime * 8.0); // �_�ő��x
   //
   //// �ԃG�t�F�N�g�̐F�i�A���t�@�̓G�b�W�̋��x + ���� + intensity�j
   //float effectStrength = edgeFade * blink * gIntensity;
   //
   //float4 alertColor = float4(1.0, 0.0, 0.0, effectStrength); // ��
   //return lerp(sceneColor, alertColor, alertColor.a);
    
   // sceneColor.g *= abs(sin(iTime));
   // sceneColor.b *= abs(sin(iTime));
    float2 uv = pin.texcoord;
    
    float4 sceneColor = texture_maps.Sample(sampler_states, uv);
    // ��ʒ��S����̋������v�Z�i0?1�j
    float2 center = float2(0.5, 0.5);
    float dist = distance(uv, center);

    // ���ł̋����i���ӂ������A���Ԃœ_�Łj
    float flash = smoothstep(0.4, 0.9, dist) * abs(sin(iTime * 5.0));

    // �ԐF�̃t���b�V�������Z
    //float4 baseColor = float4(0.0, 0.0, 0.0, 1.0); // �w�i�����Ƃ����
    float4 redFlash = float4(flash, 0.0, 0.0, 1.0);

    return sceneColor + redFlash;


    //return sceneColor; // �F������

}