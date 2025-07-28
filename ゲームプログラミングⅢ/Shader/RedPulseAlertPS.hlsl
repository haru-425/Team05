#include "fullscreenquad.hlsli"

SamplerState sampler_states : register(s0);
Texture2D texture_maps : register(t10);

float gIntensity = 1.0f; // 0.0～1.0：追跡されていないときは 0.0
float gBorderWidth = 1.0f;

// 例: 0.1 = 外周10%にエフェク


float4 main(VS_OUT pin) : SV_Target
{
    //float2 uv = pin.texcoord;


   //// 中心からの距離（0～1）
   //float2 center = float2(0.5, 0.5);
   //float dist = distance(uv, center);
   //
   //// エッジからの距離（0 = 完全な中心, 1 = 角）
   //float edgeFade = saturate((dist - (0.5 - gBorderWidth)) / gBorderWidth);
   //
   //// 時間による点滅 (sin波で 0～1 に)
   //float blink = 0.5 + 0.5 * sin(iTime * 8.0); // 点滅速度
   //
   //// 赤エフェクトの色（アルファはエッジの強度 + 明滅 + intensity）
   //float effectStrength = edgeFade * blink * gIntensity;
   //
   //float4 alertColor = float4(1.0, 0.0, 0.0, effectStrength); // 赤
   //return lerp(sceneColor, alertColor, alertColor.a);
    
   // sceneColor.g *= abs(sin(iTime));
   // sceneColor.b *= abs(sin(iTime));
    float2 uv = pin.texcoord;
    
    float4 sceneColor = texture_maps.Sample(sampler_states, uv);
    // 画面中心からの距離を計算（0?1）
    float2 center = float2(0.5, 0.5);
    float dist = distance(uv, center);

    // 明滅の強さ（周辺が強く、時間で点滅）
    float flash = smoothstep(0.4, 0.9, dist) * abs(sin(iTime * 5.0));

    // 赤色のフラッシュを加算
    //float4 baseColor = float4(0.0, 0.0, 0.0, 1.0); // 背景を黒とする例
    float4 redFlash = float4(flash, 0.0, 0.0, 1.0);

    return sceneColor + redFlash;


    //return sceneColor; // 色を強調

}