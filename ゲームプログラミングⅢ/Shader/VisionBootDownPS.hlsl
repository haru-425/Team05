#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define SHARPEN_FACTOR 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

// --- 効果終了までの時間（秒）を設定 ---
static const float effectEndTime = 2.0; // ここを変更すれば演出の長さを調整可能

float4 main(VS_OUT pin) : SV_Target
{
    float time = iTime; // 時間経過（秒）
    float2 resolution = float2(1280, 720); // 画面解像度
    float mode = 0; // 0.0: 起動 / 1.0: シャットダウン

    float startup = 1.0 - mode; // 起動時:1 → シャットダウン時:0

    // --- 効果進行度（0.0～1.0）---
    float progress = saturate(time / effectEndTime);
    float effectStrength = (mode == 0.0) ? (1.0 - progress) : progress;
    // 起動時: 1→0, シャットダウン時: 0→1

    // クロマティックアバレーション
    float offset = 0.003 * effectStrength;
    float3 color;
    color = texture_map.Sample(sampler_states[LINEAR], pin.texcoord).rgb;
   // color.r = texture_map.Sample(sampler_states[0], pin.texcoord - float2(offset, 0)).r;
   // color.g = texture_map.Sample(sampler_states[0], pin.texcoord).g;
   // color.b = texture_map.Sample(sampler_states[0], pin.texcoord + float2(offset, 0)).b;

    // 収束/拡散フォーカス
    float2 center = float2(0.5, 0.5);
    float dist = distance(pin.texcoord, center);
    float blurBias = saturate(1.0 - time * 0.5);
    float blurAmount = lerp(blurBias, 0.0, mode);
    float focus = smoothstep(0.35, 0.0, dist) * (1.0 - blurAmount * effectStrength);

    float blurOffset = 0.002 * effectStrength;
    float3 blur =
    (
        texture_map.Sample(sampler_states[LINEAR], pin.texcoord + float2(blurOffset, 0)).rgb +
        texture_map.Sample(sampler_states[LINEAR], pin.texcoord - float2(blurOffset, 0)).rgb +
        texture_map.Sample(sampler_states[LINEAR], pin.texcoord + float2(0, blurOffset)).rgb +
        texture_map.Sample(sampler_states[LINEAR], pin.texcoord - float2(0, blurOffset)).rgb
    ) * 0.25;
    color = lerp(blur, color, focus);

    // スキャンライン
 //   float scanStrength = 0.05 * effectStrength;
 //   float scan = sin((pin.texcoord.y + time * (1.5 * (startup * 2.0 - 1.0))) * resolution.y * 0.05) * scanStrength;
 //   color += scan;

    // フェードイン/アウト
    float fade = (mode == 0.0) ? progress : (1.0 - progress);
    color *= fade;

    // --- 効果終了時のなめらかな遷移 ---
    float3 baseColor = texture_map.Sample(sampler_states[LINEAR], pin.texcoord).rgb;
    if (progress >= 1.0)
    {
        if (mode == 0.0)
        {
            // 通常表示
            return float4(baseColor, 1.0);
        }
        else
        {
            // シャットダウン時は黒
            return float4(0, 0, 0, 1);
        }
    }
    else
    {
        // 効果を徐々に弱めていく
        if (mode == 0.0)
        {
            // 起動時: エフェクト→通常
            color = lerp(color, baseColor, progress);
            return float4(color, 1.0);
        }
        else
        {
            // シャットダウン時: エフェクト→黒
            color = lerp(color, float3(0, 0, 0), progress);
            return float4(color, 1.0);
        }
    }
}