#include "Custom.hlsli"
#include "Skinning.hlsli"
#include "Scene.hlsli"

VS_OUT main(float4 position : POSITION,
    float3 normal : NORMAL,
    float2 texcoord : TEXCOORD,
    float4 boneWeights : BONE_WEIGHTS,
	uint4 boneIndices : BONE_INDICES
)
{
    VS_OUT vout = (VS_OUT) 0;
    
    vout.position = SkinningPosition(position, boneWeights, boneIndices).xyz;
    vout.vertex = mul(float4(vout.position, 1), viewProjection);
    vout.texcoord = texcoord;

    vout.normal = SkinningVector(normal, boneWeights, boneIndices);
    vout.normal = normalize(vout.normal.xyz);
    
    vout.binormal = float3(0.0f, 1.0f, 0.001f); 
    vout.binormal = normalize(vout.binormal);
    vout.tangent = normalize(cross(vout.binormal, vout.normal));
    vout.binormal = normalize(cross(vout.normal, vout.tangent));


    // シャドウマップ用パラメータ計算
    {
        float4 wvpPos = mul(position, lightViewProjection);
        
        //NDC系からUV座標を算出
        wvpPos /= wvpPos.w;
        wvpPos.y = -wvpPos.y;
        wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
        vout.shadowTexcoord = wvpPos.xyz;
    }
    
    return vout;
}