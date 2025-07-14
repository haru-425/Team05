#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define SHARPEN_FACTOR 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

//float rand(float2 co)
//{
//    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
//}
float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}



// �s�N�Z���V�F�[�_�̃��C���֐�
// sharpenMask�֐��Ōv�Z�����F�����̂܂܏o��
float4 main(VS_OUT pin) : SV_Target
{
 //  float4 baseColor = texture_map.Sample(sampler_states[0], pin.texcoord);
 //
 //
 //
 //  // ��ꂽ�c���̈ʒu�ƕ�
 //  float glitchLineX = 0.3 + 0.2 * sin(iTime * 1.3); // �ʒu�����Ԃł�炮
 //  float glitchWidth = 0.02;
 //
 //  // uv���W�����͈͓̔����ǂ���
 //  if (abs(pin.texcoord.x - glitchLineX) < glitchWidth)
 //  {
 //      // RGB�̂���ɂ���ĐF����
 //      float2 offset = float2(0.01, 0.0);
 //      float r = texture_map.Sample(sampler_states[0], pin.texcoord - offset).r;
 //      float g = texture_map.Sample(sampler_states[0], pin.texcoord).g;
 //      float b = texture_map.Sample(sampler_states[0], pin.texcoord + offset).b;
 //
 //      // �m�C�Y�ł����
 //      float noise = rand(pin.position.xy + iTime * 50.0);
 //      float flicker = step(0.5, noise); // �_�Ō���
 //
 //      baseColor = float4(r, g, b, 1.0) * flicker;
 //  }
 //
 //  return baseColor;
    float time = iTime;
    float2 resolution = float2(1280, 720);
    int numLines = 1000; // �c���̖{���i������ύX�\�j
    float lineWidth = 0.001; // �e�c���̕�
    float glitchIntensity = 5; // y�����̈����L�΂��{��
    float glitchThreshold = 0.98; // �����m��s

      // �X�L�������C���P�ʂŃ����_���ɏ���
//    float2 uv = pin.texcoord; // �e�N�X�`�����W
//    float scanline = floor(uv.x * 1280.0);
//    float stretchChance = rand(float2(scanline, floor(iTime)));
//
//    if (stretchChance > 0.97)
//    {
//        float stretchAmount = 0.5 + 1.5 * rand(float2(scanline, iTime * 0.5));
//        uv.y = frac(uv.y * stretchAmount * 100);
//    }
//
//    float4 color = texture_map.Sample(sampler_states[0], uv);
//
//    return color;

    float2 distortedUV = pin.texcoord;
    distortedUV = pin.texcoord;
    // �e�c�����Ƃɏ���
    for (int i = 0; i < numLines; i++)
    {
        float lineSeed = rand(float2(i * 17.0, floor(time)));
        float lineX = lineSeed; // 0?1�̈ʒu�Ƀ����_���z�u

        if (abs(pin.texcoord.x - lineX) < lineWidth)
        {
            float stretchChance = rand(float2(i * 31.1, floor(time * 0.5)));
            if (stretchChance > glitchThreshold)
            {
                float stretchAmount = 0.5 + glitchIntensity * rand(float2(i * 13.7, time));
                distortedUV.y = frac(pin.texcoord.y * stretchAmount);
            }
        }
    }

    return texture_map.Sample(sampler_states[LINEAR], distortedUV); // �c��UV�ŐF���T���v�����O

}


