//--------------------------------------------
//	�����o�[�g�g�U���ˌv�Z�֐�
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�
float3 CalcLambert(float3 N, float3 L, float3 C, float3 K)
{
    float power = saturate(dot(N, L));
    return C * power * K;
}

//--------------------------------------------
//	�t�H���̋��ʔ��ˌv�Z�֐�
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// E:�����x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�
float3 CalcPhongSpecular(float3 N, float3 L, float3 V, float3 C, float3 K)
{
    float3 R = reflect(L, N);
    float power = max(dot(V, R), 0);
    power = pow(power, 128);
    return C * power * K;
}

//--------------------------------------------
//	�n�[�t�����o�[�g�g�U���ˌv�Z�֐�
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�
float3 ClacHalfLambert(float3 N, float3 L, float3 C, float3 K)
{
    float D = saturate(dot(N, L) * 0.5f + 0.5f);
    return C * D * K;
}

//--------------------------------------------
// �������C�g
//--------------------------------------------
// N:�@��(���K���ς�)
// E:���_�����x�N�g��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C :���C�g�F
// RimPower : �������C�g�̋���(�����l�̓e�L�g�[�Ȃ̂Ŏ����Őݒ肷�邪�g)
float3 CalcRimLight(float3 N, float3 V, float3 L, float3 C, float RimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(N, V));
    return C * pow(rim, RimPower) * saturate(dot(L, V));
}

//--------------------------------------------
// �����v�V�F�[�f�B���O
//--------------------------------------------
// tex:�����v�V�F�[�f�B���O�p�e�N�X�`��
// samp:�����v�V�F�[�f�B���O�p�T���v���X�e�[�g
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�
float3 CalcRampShading(Texture2D tex, SamplerState samp, float3 N, float3 L, float3 C, float3 K)
{
    float D = saturate(dot(N, L) * 0.5f + 0.5f);
    float Ramp = tex.Sample(samp, float2(D, 0.5f)).r;
    return C * Ramp * K.rgb;
}

//--------------------------------------------
// �������C�e�B���O
//--------------------------------------------
// normal:�@��(���K���ς�)
// up:������i�Е��j
// sky_color:��(��)�F
// ground_color:�n��(��)�F
// hemisphere_weight:�d��
float3 CalcHemiSphereLight(float3 normal, float3 up, float3 sky_color, float3 ground_color, float4 hemisphere_weight)
{
    float factor = dot(normal, up) * 0.5f + 0.5f;
    return lerp(ground_color, sky_color, factor) * hemisphere_weight.x;
}

//--------------------------------------------
//	�t�H�O
//--------------------------------------------
//color:���݂̃s�N�Z���F
//fog_color:�t�H�O�̐F
//fog_range:�t�H�O�͈̔͏��
//eye_length:���_����̋���
float4 CalcFog(in float4 color, float4 fogColor, float2 fogRange, float eyeLength)
{
    float fogAlpha = saturate((eyeLength - fogRange.x) / (fogRange.y - fogRange.x));
    return lerp(color, fogColor, fogAlpha);
}

//--------------------------------------------
//	�t���l����
//--------------------------------------------
//F0	: �������ˎ��̔��˗�
//VdotH	: �����x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g��
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - VdotH, 0.0f, 1.0f), 5.0f);
}

static const float PI = 3.14159265359f;
//--------------------------------------------
//	�g�U����BRDF(���K�������o�[�g�̊g�U����)
//--------------------------------------------
//VdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//fresnelF0	: �������ˎ��̃t���l�����ːF
//diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
float3 DiffuseBRDF(float VdotH, float3 fresnelF0, float3 diffuseReflectance)
{
    return (1.0f - CalcFresnel(fresnelF0, VdotH)) * (diffuseReflectance / PI);
}

//--------------------------------------------
//	�@�����z�֐�
//--------------------------------------------
//NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j�̓���
//roughness : �e��
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float b = (NdotH * NdotH) * (a - 1.0f) + 1.0f;
    return a / (PI * b * b);
}

//--------------------------------------------
//	�􉽌������̎Z�o
//--------------------------------------------
//NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
//NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
//roughness : �e��
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = NdotL / (NdotL * (1.0 - r) + r);
    float masking = NdotV / (NdotV * (1.0 - r) + r);
    return shadowing * masking;
}

//--------------------------------------------
//	���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
//--------------------------------------------
//NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
//NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
//NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//VdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
//fresnelF0	: �������ˎ��̃t���l�����ːF
//roughness	: �e��
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	//	D��(�@�����z)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
	//	G��(�􉽌�����)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	//	F��(�t���l������)
    float3 F = (CalcFresnel(fresnelF0, VdotH));

    return (D * F * G) / (4.0 * NdotV * NdotL + 1e-5);
}

//--------------------------------------------
//	���ڌ��̕����x�[�X���C�e�B���O
//--------------------------------------------
//diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
//F0					: �������ˎ��̃t���l�����ːF
//normal				: �@���x�N�g��(���K���ς�)
//eyeVector				: ���_�Ɍ������x�N�g��(���K���ς�)
//lightVector			: �����Ɍ������x�N�g��(���K���ς�)
//lightColor			: ���C�g�J���[
//roughness				: �e��
void DirectBDRF(float3 diffuseReflectance,
				float3 F0,
				float3 N,
				float3 V,
				float3 L,
                float3 H,
				float3 lightColor,
				float roughness,
				out float3 outDiffuse,
				out float3 outSpecular)
{
    //float3 N = normal;
    //float3 L = -lightVector;
    //float3 V = -eyeVector;
    //float3 H = normalize(L + V);

    float NdotV = max(0.0001f, dot(N, V));
    float NdotL = max(0.0001f, dot(N, L));
    float NdotH = max(0.0001f, dot(N, H));
    float VdotH = max(0.0001f, dot(V, H));

    float3 irradiance = lightColor * NdotL;

    //	�g�U����BRDF
    outDiffuse = DiffuseBRDF(VdotH, F0, diffuseReflectance) * irradiance;

	//	���ʔ���BRDF
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}


float3 ClosestPointOnLine(float3 p, float3 start, float3 end)
{
    float3 lineDir = end - start;
    float lineLengthSq = dot(lineDir, lineDir);
    float t = dot(p -start, lineDir) / lineLengthSq;
    t = saturate(t);
    return start + t * lineDir;
}
