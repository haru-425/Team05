#include "Scene.hlsli"
#include "Custom.hlsli"
#include "Function.hlsli"

cbuffer CbMesh : register(b1)
{
    float4 materialColor;
};

Texture2D Textures[6] : register(t0);
Texture2D shadowMap : register(t8);

SamplerState LinearSampler : register(s0);
SamplerState ShadowSamplerState : register(s2);

#define BASECOLOR_TEXTURE    0
#define NORMAL_TEXTURE       1
#define ROUGHNESS_TEXTURE    2
#define METALNESS_TEXTURE    3
#define EMISIVE_TEXTURE      4
#define OCCLUSION_TEXTURE    5

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
    roughness = max(0, roughness);

    float metalness = Textures[METALNESS_TEXTURE].Sample(LinearSampler, pin.texcoord).r;
    metalness = max(0, metalness);
   
    // occlusion ---------------------------------------------------------
    float3 occlusion = Textures[OCCLUSION_TEXTURE].Sample(LinearSampler, pin.texcoord).rrr;
    const float occlusionStrength = 1.0f;
    
    // �t���l�����˗��̏����l�i������͍Œ�4%�j
    float4 albedo = color;
    float3 F0 = lerp(0.04f, albedo.rgb, metalness); //	�������ˎ��̃t���l�����˗�(������ł��Œ�4%�͋��ʔ��˂���

    // ���������ƌ��������̌v�Z
    float3 L = normalize(-lightDirection.xyz);
    float3 V = normalize(cameraPosition.xyz - pin.position.xyz);
    
    float3 H = normalize(V + L);
    float VdotH = saturate(dot(V, H));
  
    float4 ka = { 0.2f, 0.2f, 0.2f, 1.0f }; // �����ɑ΂��锽�ˌW��
    float4 kd = { 0.8f, 0.8f, 0.8f, 1.0f }; // �g�U���ɑ΂��锽�ˌW��
    float4 ks = { 1.0f, 1.0f, 1.0f, 1.0f }; // ���ʌ��ɑ΂��锽�ˌW��    

    // �V���h�E�K�p
    //color.rgb = ApplyShadowToObject(pin, color.rgb);
   
    // �����̌v�Z
    float3 ambient = ambientColor.rgb * ka.rgb;

    // �_�����̊g�U�E���ʔ��˂̏�����
    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;

    // �_�����̃��[�v����
    for (int i = 0; i < 34; ++i)
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
        
        //return float4(NdotL.xxx, 1);

        // �g�U���˂Ƌ��ʔ��˂̉��Z
        pointDiffuse += DiffuseBRDF(VdotH, F0, kd.rgb) * attenuation * pointLights[i].color.rgb;
        pointSpecular += SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * attenuation * pointLights[i].color.rgb;
        
        pointDiffuse = max(0, pointDiffuse);
        pointSpecular = max(0, pointSpecular);
    }
    float3 torusDiffuse = 0, torusSpecular = 0;
    for (i = 0; i < 13; ++i)
    {
        // �g�[���X���S����s�N�Z���ւ̃x�N�g��
        float3 toPixel = pin.position.xyz - torusLights[i].position.xyz;
      
        // �g�[���X�̎������ɓ��e���āA���S�~�̕��ʏ�ɓ��e
        float3 projected = toPixel - dot(toPixel, torusLights[i].direction.xyz) * torusLights[i].direction.xyz;
      
        // ���S�~�̋����ƁA�g�[���X�\�ʂ̍ŋߓ_
        float distToCenterCircle = length(projected);
        float3 centerOnTorus = torusLights[i].position.xyz + normalize(projected) * torusLights[i].majorRadius;

        // �s�N�Z������g�[���X�\�ʂ܂ł̋���
        float3 toCenter = pin.position.xyz - centerOnTorus;
        float distToTorusSurface = length(toCenter);
        
        // �g�[���X�͈̔͊O�Ȃ�X�L�b�v
        if (abs(distToCenterCircle - torusLights[i].majorRadius) > torusLights[i].minorRadius)
            continue;
        if (distToTorusSurface > torusLights[i].minorRadius)
            continue;
        
        // ���������i���ɍő勗��10.0f�Ƃ���j
        float len = length(centerOnTorus - pin.position.xyz);
        float attenuateLength = saturate(1.0f - len / torusLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        
        //return float4(attenuation.xxx, 1);
        
        // ���C�g�����ƃn�[�t�x�N�g��
        float3 lightDir = normalize(centerOnTorus - pin.position.xyz);
        float3 H = normalize(V + lightDir);
        
        // �e��h�b�g��
        float NdotL = saturate(dot(N, lightDir));
        float NdotV = saturate(dot(N, V));
        float NdotH = saturate(dot(N, H));
        float VdotH = saturate(dot(V, H));
        
        // �g�U�E���ʔ���
        torusDiffuse += DiffuseBRDF(VdotH, F0, kd.rgb) * attenuation * torusLights[i].color.rgb;
        torusSpecular += SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * attenuation * torusLights[i].color.rgb;
        
        torusDiffuse = max(0, torusDiffuse);
        torusSpecular = max(0, torusSpecular);
    }
    // �������̎���
    float3 lineDiffuse = 0, lineSpecular = 0;
    for (i = 0; i < 45; ++i)
    {
        for (int s = 0; s < 6; ++s)
        {
            float t = s / 5.0f;
            float3 pointOnLine = lerp(lineLights[i].start.xyz, lineLights[i].end.xyz, t);
             
            float3 LP = normalize(pointOnLine - pin.position.xyz);
            float len = length(pointOnLine - pin.position.xyz);
            if (len >= lineLights[i].range)
                continue;
             
            float attenuation = pow(saturate(1.0f - len / lineLights[i].range), 2.0f);
             
            float3 H = normalize(V + LP);
            float NdotL = saturate(dot(N, LP));
            float NdotV = saturate(dot(N, V));
            float NdotH = saturate(dot(N, H));
            float VdotH = saturate(dot(V, H));
             
            lineDiffuse += DiffuseBRDF(VdotH, F0, kd.rgb) * attenuation * lineLights[i].color.rgb;
            lineSpecular += SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * attenuation * lineLights[i].color.rgb;
        }
        lineDiffuse = max(0, lineDiffuse);
        lineSpecular = max(0, lineSpecular);
    }

    float3 totalDiffuse = (pointDiffuse + lineDiffuse + torusDiffuse) * power;
    float3 totalSpecular = (pointSpecular + lineSpecular + torusSpecular);
    
    //	�Օ�����
    totalDiffuse = lerp(totalDiffuse, totalDiffuse * occlusion, occlusionStrength);
    totalSpecular = lerp(totalSpecular, totalSpecular * occlusion, occlusionStrength);
    
    color.rgb *= totalDiffuse;
    color.rgb += totalSpecular;
    
    color.rgb += emisive;
    
    //color.a = lerp(color.a, color.a * occlusion.r, occlusionStrength);

#if 1
    // �t�H�O����
    float eyeLength = length(pin.position.xyz - cameraPosition.xyz);
    color = CalcFog(color, fogColor, fogRange.xy, eyeLength);
    
#endif  

    // �K���}�␳�isRGB��Ԃ֖߂��j
    color = pow(color, 1.0f / GammaFactor);
        
    return float4(color.rgb + ambient, color.a);
}

