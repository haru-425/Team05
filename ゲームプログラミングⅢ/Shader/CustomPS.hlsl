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
            
            
            // ガウシアン重み（中心に近いほど重く）
            float weight = exp(-dot(offset, offset) * 150.0); // 150.0 はぼかし強度（調整可）            
            
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
    
    // 境界のぼかし処理
    // 境目の色（例：青っぽい）
    float3 shadowed = color.rgb * shadowColor.rgb;
    float3 edgeColor = float3(0.57f, 0.3f, 0.031f);
    
    // シャドウの強さに応じてブレンド
    float3 edgeBlend = lerp(edgeColor, color.rgb, shadowFactor);
    
    float edgeFactor = smoothstep(0.15, 1.8f, shadowFactor);

    float3 gradientColor = lerp(shadowed, edgeColor, edgeFactor);
    color.rgb = lerp(gradientColor, color.rgb, shadowFactor);
    
    return float3(color.rgb);
}

float4 main(VS_OUT pin) : SV_TARGET
{
	// ガンマ補正係数
    static const float GammaFactor = 2.2f;

    // BaseColor ---------------------------------------------------------    
    float4 color = Textures[BASECOLOR_TEXTURE].Sample(LinearSampler, pin.texcoord) * materialColor;
    color.rgb = pow(color.rgb, GammaFactor);
    
    // Normal ------------------------------------------------------------
    // ノーマルマップから取得し、 ワールド空間へ変換
    float3x3 mat = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
    float3 N = Textures[NORMAL_TEXTURE].Sample(LinearSampler, pin.texcoord).rgb;
    N = normalize(mul(N * 2.0f - 1.0f, mat));

    // emisive -----------------------------------------------------------
    float3 emisive = Textures[EMISIVE_TEXTURE].Sample(LinearSampler, pin.texcoord).rgb;
    
    // Roughness, metalness ----------------------------------------------
    float roughness = Textures[ROUGHNESS_TEXTURE].Sample(LinearSampler, pin.texcoord).r;
    float metalness = Textures[METALNESS_TEXTURE].Sample(LinearSampler, pin.texcoord).r;
   
    // フレネル反射率の初期値（非金属は最低4%）
    float4 albedo = color;
    float3 F0 = lerp(0.04f, albedo.rgb, metalness); //	垂直反射時のフレネル反射率(非金属でも最低4%は鏡面反射する
    F0 = (0.04f, 0.04f, 0.04f);
    
    // 視線方向と光源方向の計算
    float3 L = normalize(-lightDirection.xyz);
    float3 V = normalize(cameraPosition.xyz - pin.position.xyz);
    
    float3 H = normalize(V + L);
    float VdotH = saturate(dot(V, H));
  
    float4 ka = { 0.2f, 0.2f, 0.2f, 1.0f }; // 環境光に対する反射係数
    float4 kd = { 0.8f, 0.8f, 0.8f, 1.0f }; // 拡散光に対する反射係数
    float4 ks = { 1.0f, 1.0f, 1.0f, 1.0f }; // 鏡面光に対する反射係数    

    // シャドウ適用
    color.rgb = ApplyShadowToObject(pin, color.rgb);
   
    // 環境光の計算
    float3 ambient = ambientColor.rgb * ka.rgb;

    // 平行光源
    float3 diffuse = DiffuseBRDF(VdotH, F0, kd.rgb);
    //float3 diffuse = CalcLambert(N, L, float3(1, 1, 1), kd.rgb);
    color.rgb *= diffuse;
    
    // 点光源の拡散・鏡面反射の初期化
    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;

    // 点光源のループ処理
    for (int i = 0; i < 47; ++i)
    {
        float3 LP = pointLights[i].position.xyz - pin.position.xyz;
        float len = length(LP);
        if (len >= pointLights[i].range)
            continue;
        // 距離減衰の計算
        float attenuateLength = saturate(1.0f - len / pointLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        LP /= len;
        
        // ハーフベクトルの計算
        float3 H = normalize(V + LP);
        
        // 各種ドット積の計算
        float NdotL = saturate(dot(N, LP));
        float NdotV = saturate(dot(N, V));
        float NdotH = saturate(dot(N, H));
        float VdotH = saturate(dot(V, H));

        // 拡散反射と鏡面反射の加算
        pointDiffuse += DiffuseBRDF(VdotH, F0, kd.rgb) * attenuation * pointLights[i].color.rgb;
        pointSpecular += SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * attenuation * pointLights[i].color.rgb;
    }
    
    // 線光源の実装
    float3 lineDiffuse = 0, lineSpecular = 0;
    for (i = 0; i < 42; ++i)
    {
        float3 closetPoint = ClosestPointOnLine(pin.position.xyz, lineLights[i].start.xyz, lineLights[i].end.xyz);
        float3 LP = normalize(closetPoint - pin.position.xyz);
        float dist = length(closetPoint - pin.position.xyz);
        
        float attenuateLength = saturate(1.0f - dist / lineLights[i].range);
        float attenuation = attenuateLength * attenuateLength;
        
        // ハーフベクトルの計算
        float3 H = normalize(V + LP);
        
        // 各種ドット積の計算
        float NdotL = saturate(dot(N, LP));
        float NdotV = saturate(dot(N, V));
        float NdotH = saturate(dot(N, H));
        float VdotH = saturate(dot(V, H));
        
        lineDiffuse += DiffuseBRDF(VdotH, F0, kd.rgb) * power * attenuation * lineLights[i].color.rgb;
        lineSpecular += SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * power * attenuation * lineLights[i].color.rgb;

    }
    
    // 点光源の影響を加算
    color.rgb += color.rgb * (pointDiffuse + lineDiffuse) * power;
    color.rgb += (pointSpecular + lineSpecular) * power;
    
    color.rgb += emisive;

#if 1
    // フォグ処理
    float eyeLength = length(pin.position.xyz - cameraPosition.xyz);
    color = CalcFog(color, fogColor, fogRange.xy, eyeLength);
    
#endif  

    // ガンマ補正（sRGB空間へ戻す）
    color = pow(color, 1.0f / GammaFactor);
        
    return float4(color.rgb + ambient, color.a);
}

