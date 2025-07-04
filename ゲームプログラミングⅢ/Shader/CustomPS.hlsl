#include "Scene.hlsli"
#include "Custom.hlsli"
#include "Function.hlsli"

cbuffer CbMesh : register(b1)
{
    float4 materialColor;
};

Texture2D Textures[5] : register(t0);
Texture2D shadowMap : register(t8);

SamplerState LinearSampler : register(s0);
SamplerState ShadowSamplerState : register(s2);

#define BASECOLOR_TEXTURE    0
#define NORMAL_TEXTURE       1
#define ROUGHNESS_TEXTURE    2
#define METALNESS_TEXTURE    3
#define EMISIVE_TEXTURE      4

float ComputeShadowPCF_Gaussian(float3 shadowCoord)
{
    static const float2 shadowTexelSize = float2(1.0 / 2048.0, 1.0 / 2048.0);
    
    float2 uv = shadowCoord.xy;
    float depth = shadowCoord.z;

    float shadow = 0.0;
    float totalWeight = 0.0;
    
    int kernelSize = 5;
    int halfKernel = kernelSize / 2;

    for (int x = -halfKernel; x <= halfKernel; ++x)
    {
        for (int y = -halfKernel; y <= halfKernel; ++y)
        {
            float2 offset = float2(x, y) * shadowTexelSize;
            
            
            // �K�E�V�A���d�݁i���S�ɋ߂��قǏd���j
            float weight = exp(-dot(offset, offset) * 150.0); // 150.0 �͂ڂ������x�i�����j            
            
            float sampledDepth = shadowMap.Sample(ShadowSamplerState, uv + offset).r;
            float visibility = (depth - shadowBias > sampledDepth) ? 0.0 : 1.0;

            
            shadow += visibility * weight;
            totalWeight += weight;

        }
    }

    return shadow / totalWeight;
}


float3 ApplyShadowToObject(VS_OUT pin, float3 color)
{
    float depth = shadowMap.Sample(ShadowSamplerState, pin.shadowTexcoord.xy).r;
    
    if (pin.shadowTexcoord.z - depth > shadowBias)
    {
        color.rgb *= shadowColor.rgb;
    }
    
    float shadowFactor = ComputeShadowPCF_Gaussian(pin.shadowTexcoord);
    color.rgb = lerp(color.rgb * shadowColor.rgb, color.rgb, shadowFactor);
    
    // ���E�̂ڂ�������
    // ���ڂ̐F�i��F���ۂ��j
    float3 shadowed = color.rgb * shadowColor.rgb;
    float3 edgeColor = float3(0.57f, 0.3f, 0.031f);
    
    // �V���h�E�̋����ɉ����ău�����h
    float3 edgeBlend = lerp(edgeColor, color.rgb, shadowFactor);
    
    float edgeFactor = smoothstep(0.15, 1.8f, shadowFactor);

    float3 gradientColor = lerp(shadowed, edgeColor, edgeFactor);
    color.rgb = lerp(gradientColor, color.rgb, shadowFactor);
    
    return float3(color.rgb);
}

float4 main(VS_OUT pin) : SV_TARGET
{
	// �K���}�␳�W��
    static const float GammaFactor = 2.2f;

    // BaseColor ---------------------------------------------------------    
    float4 color = Textures[BASECOLOR_TEXTURE].Sample(LinearSampler, pin.texcoord) * materialColor;
    color.rgb = pow(color.rgb, GammaFactor);
    
    // Normal ------------------------------------------------------------
    // �m�[�}���}�b�v����擾���A ���[���h��Ԃ֕ϊ�
    float3x3 mat = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
    float3 N = Textures[NORMAL_TEXTURE].Sample(LinearSampler, pin.texcoord).rgb;
    N = normalize(mul(N * 2.0f - 1.0f, mat));

    // emisive -----------------------------------------------------------
    float3 emisive = Textures[EMISIVE_TEXTURE].Sample(LinearSampler, pin.texcoord).rgb;
    
    // Roughness, metalness ----------------------------------------------
    float roughness = Textures[ROUGHNESS_TEXTURE].Sample(LinearSampler, pin.texcoord).r;
    float metalness = Textures[METALNESS_TEXTURE].Sample(LinearSampler, pin.texcoord).r;
   
    // �t���l�����˗��̏����l�i������͍Œ�4%�j
    float4 albedo = color;
    float3 F0 = lerp(0.04f, albedo.rgb, metalness); //	�������ˎ��̃t���l�����˗�(������ł��Œ�4%�͋��ʔ��˂���
    F0 = (0.04f, 0.04f, 0.04f);
    
    // ���������ƌ��������̌v�Z
    float3 L = normalize(-lightDirection.xyz);
    float3 V = normalize(cameraPosition.xyz - pin.position.xyz);
    
    float3 H = normalize(V + L);
    float VdotH = saturate(dot(V, H));
  
    float4 ka = { 0.2f, 0.2f, 0.2f, 1.0f }; // �����ɑ΂��锽�ˌW��
    float4 kd = { 0.8f, 0.8f, 0.8f, 1.0f }; // �g�U���ɑ΂��锽�ˌW��
    float4 ks = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���ʌ��ɑ΂��锽�ˌW��    

    // �V���h�E�K�p
    color.rgb = ApplyShadowToObject(pin, color.rgb);
   
    // �����̌v�Z
    float3 ambient = ambientColor.rgb * ka.rgb;

    // ���s����
    float3 diffuse = DiffuseBRDF(VdotH, F0, kd.rgb);
    //float3 diffuse = CalcLambert(N, L, float3(1, 1, 1), kd.rgb);
    color.rgb *= diffuse;
    
    // �_�����̊g�U�E���ʔ��˂̏�����
    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;

    // �_�����̃��[�v����
    for (int i = 0; i < 47; ++i)
    {
        float3 LP = pointLights[i].position.xyz - pin.position.xyz;
        float len = length(LP);
        if (len >= pointLights[i].range)
            continue;
        // ���������̌v�Z
        float attenuateLength = saturate(1.0f - len / pointLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        
        // �n�[�t�x�N�g���̌v�Z
        float3 H = normalize(V + LP);
        
        // �e��h�b�g�ς̌v�Z
        float NdotL = saturate(dot(N, LP));
        float NdotV = saturate(dot(N, V));
        float NdotH = saturate(dot(N, H));
        float VdotH = saturate(dot(V, H));

        // �g�U���˂Ƌ��ʔ��˂̉��Z
        pointDiffuse += DiffuseBRDF(VdotH, F0, kd.rgb) * attenuation * pointLights[i].color.rgb;
        pointSpecular += SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * attenuation * pointLights[i].color.rgb;
    }
    
    // �������̎���
    float3 lineDiffuse = 0, lineSpecular = 0;
    for (i = 0; i < 42; ++i)
    {
        float3 closetPoint = ClosestPointOnLine(pin.position.xyz, lineLights[i].start.xyz, lineLights[i].end.xyz);
        float3 LP = normalize(closetPoint - pin.position.xyz);
        float dist = length(closetPoint - pin.position.xyz);
        
        float attenuateLength = saturate(1.0f - dist / lineLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        
        // �n�[�t�x�N�g���̌v�Z
        float3 H = normalize(V + LP);
        
        // �e��h�b�g�ς̌v�Z
        float NdotL = saturate(dot(N, LP));
        float NdotV = saturate(dot(N, V));
        float NdotH = saturate(dot(N, H));
        float VdotH = saturate(dot(V, H));
        
        lineDiffuse += DiffuseBRDF(VdotH, F0, kd.rgb) * power * attenuation * lineLights[i].color.rgb;
        lineSpecular += SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * power * attenuation * lineLights[i].color.rgb;

    }
    
    // �_�����̉e�������Z
    color.rgb += color.rgb * (pointDiffuse + lineDiffuse) * power;
    color.rgb += (pointSpecular + lineSpecular) * power;
    
    color.rgb += emisive;

#if 1
    // �t�H�O����
    float eyeLength = length(pin.position.xyz - cameraPosition.xyz);
    color = CalcFog(color, fogColor, fogRange.xy, eyeLength);
    
#endif  

    // �K���}�␳�isRGB��Ԃ֖߂��j
    color = pow(color, 1.0f / GammaFactor);
        
    return float4(color.rgb + ambient, color.a);
}

