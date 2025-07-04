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

    float time = fmod(iTime, 1000.0); // 時間をループさせる
    float noise = rand(pin.texcoord * 100 + time * 20.0); // 時間とUVに基づくノイズ
    //float frameSeed = floor(time * 60.0); // フレーム単位のシード（60FPS想定）

    // フレームごとのランダム値を生成
    //float flickerNoise = rand(float2(frameSeed, pin.texcoord.y * 100.0));


    float flicker = lerp(1.0, 1.5, noise); // 輝度の揺らぎ
    //float flicker = lerp(0.8, 1.2, flickerNoise); // 輝度の揺らぎ

    float4 color = texture_map.Sample(sampler_states[0], pin.texcoord);
    color.rgb *= flicker; // 輝度にノイズを加える
    color.a = 1.0; // アルファ値を1.0に設定（透過しないようにする）
    return color;


}