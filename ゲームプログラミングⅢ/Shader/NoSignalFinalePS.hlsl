#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define SHARPEN_FACTOR 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

// 開始時間（秒）と継続時間（秒）を設定
static const float startTime = 1.0; // 例: 1秒後に開始
static const float duration = 0.3; // 例: 0.3秒かけて終了

float4 main(VS_OUT pin) : SV_Target
{
    float2 uv = pin.texcoord;
    float time = iTime; // グローバル変数iTimeから経過時間（秒）を取得

    // エフェクト進行度（0.0～1.0）を計算
    float progress = saturate((time - startTime) / duration);

    // 画面の中心を取得
    float2 center = float2(0.5, 0.5);
    float2 delta = uv - center;

    // 時間が進むにつれて、中央へ収束して線状になっていく
    float collapse = pow(progress, 2.0); // 緩やかな開始、加速する収束
    delta.y /= (1.0 - collapse); // 横方向が収束
    delta.x *= (1.0 - collapse * 0.1); // 縦方向はわずかに収束

    float2 displacedUV = center + delta;
    float4 color = texture_map.Sample(sampler_states[0], displacedUV);

    // --- スキャンライン効果 ---
    float scanline = 0.85 + 0.15 * sin(uv.y * 1080.0 * 3.14159); // 1080は解像度に応じて調整
    color.rgb *= scanline;

    // --- ホワイトアウト効果（進行度0.9以降で発動） ---
    float whiteout = smoothstep(0.9, 1.0, progress);
    color.rgb = lerp(color.rgb, float3(1.0, 1.0, 1.0), whiteout);

    // --- 完全終了時は黒 ---
    if (progress >= 1.0)
    {
        color.rgb = float3(0.0, 0.0, 0.0);
    }

    return float4(color.rgb, 1);
}