#include"Sprite.hlsli"
cbuffer NoiseSettings : register(b1)
{
    float time;
    float strength;//�m�C�Y�̋���
    float dummy1;
}
Texture2D inputTex : register(t0);
SamplerState samp : register(s0);
// �[�������i�m�C�Y�j����
float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float4 main(VS_OUT pin):SV_TARGET
{  
       //�����_���ɐ����C�����ɂ��炷
    float offset = 0.0;
    float lineNoise = rand(float2(pin.texcoord.y * 100, time));

    if (lineNoise < 0.1f + strength * 0.5f)
    {
        //�����_���ɍ��E�ɃI�t�Z�b�g(�����ɂ��)
        offset = rand(float2(pin.texcoord.y * 50.0f, time * 0.5f)) * 0.2f * strength;
        offset = (rand(float2(pin.texcoord.y * 25.0f, time)) > 0.5f) ? offset : -offset;
    }
    float2 uv_offset = pin.texcoord;
    uv_offset.x += offset;
    
    // clamp���Ĕ͈͂𐧌��i�͂ݏo���ƍ��ɂȂ�̂�h���j
    float2 uv_r = pin.texcoord + float2(offset, 0.0);
    float2 uv_g = pin.texcoord;
    float2 uv_b = pin.texcoord - float2(offset, 0.0);

    //float3 col;
    //col.r = inputTex.Sample(samp, uv_r);
    //col.g = inputTex.Sample(samp, uv_g);
    //col.b = inputTex.Sample(samp, uv_b);

    //float a_r = inputTex.Sample(samp, uv_r).a;
    //float a_g = inputTex.Sample(samp, uv_g).a;
    //float a_b = inputTex.Sample(samp, uv_b).a;
    //float a = (a_r + a_g + a_b) / 3.0;

    //return float4(col, a);
    //�e�N�X�`�����擾
    
    float2 offsetR = float2(0.05f, 0);
    float2 offsetB = float2(-0.05f, 0);

    float4 colCenter = inputTex.Sample(samp, uv_offset);
    float4 colR = inputTex.Sample(samp, uv_offset + offsetR);
    float4 colB = inputTex.Sample(samp, uv_offset + offsetB);

    float3 finalColor;
    finalColor.r = colR.r;
    finalColor.g = colCenter.g;
    finalColor.b = colB.b;

    float finalAlpha = (colR.a + colCenter.a + colB.a) / 3.0;

    return float4(finalColor, finalAlpha);

}