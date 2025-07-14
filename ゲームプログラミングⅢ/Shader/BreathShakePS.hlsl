#include "FullScreenQuad.hlsli"
#define CA_AMT 1.01
#define POINT 0
#define LINEAR 1
#define PI 3.14159265358979323846
SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

/**
 * @brief 呼吸や心拍の動きを模倣した画面揺れ・ズーム効果を付与するピクセルシェーダ
 *
 * このシェーダは、呼吸の周期的な動きと心拍の瞬間的な揺れを組み合わせて、
 * 画面全体に自然な揺れやズーム効果を与えます。
 * 呼吸は非対称な波形で表現し、吸気はゆるやか、呼気は急激な動きとなります。
 * 心拍は周期的なパルスとして、画面に小さな揺れを加えます。
 *
 * @param pin 頂点シェーダから渡される出力構造体。テクスチャ座標(texcoord)などを含みます。
 * @return float4 最終的な色。アルファ値は常に1.0です。
 */
float4 main(VS_OUT pin) : SV_TARGET
{
    // グローバル変数iTimeから経過時間（秒）を取得
    float time = iTime;

    // ピクセルのテクスチャ座標（0～1の範囲）
    float2 uv = pin.texcoord;

    // --- 呼吸パラメータ設定 ---
    float breathCycle = 4.0; // 呼吸の1周期（秒）
    float verticalAmplitude = 0.003; // 呼吸による上下揺れの振幅
    float zoomAmplitude = 0.01; // 呼吸によるズーム（拡大縮小）の振幅

    // 呼吸波形の生成（非対称：吸気はゆるやか、呼気は急激）
    float t = frac(time / breathCycle); // 現在の呼吸サイクル内の位置（0～1）
    float asymBreath;
    if (t < 0.6)
    {
        // 吸気：0～0.6の間はsinカーブでゆるやかに上昇
        asymBreath = sin(t / 0.6 * PI * 0.5);
    }
    else
    {
        // 呼気：0.6～1.0の間は急激に下降
        asymBreath = -pow((t - 0.6) / 0.4, 0.5);
    }

    // 呼吸による上下揺れ（Y方向オフセット）
    float offsetY = asymBreath * verticalAmplitude;

    // 呼吸によるズーム（拡大縮小）
    float zoom = 1.0 + asymBreath * zoomAmplitude;
    float2 center = float2(0.5, 0.5); // 画面中心座標
    // 画面中心を基準に拡大縮小
    uv = center + (uv - center) / zoom;

    // --- 心拍パラメータ設定 ---
    float heartRate = 75.0 / 60.0; // 心拍数（BPM→Hz）
    // 心拍の波形（sin波で周期的に発生）
    float pulse = sin(time * 2 * PI * heartRate);
    // 拍動のときだけ有効（負の値は0にする）
    pulse = max(pulse, 0.0);
    // シャープで小さな揺れ（心拍の瞬間的な動き）
    pulse = pulse * pulse * 0.001;
    // 揺れの方向を時間で回転させることで、より自然な動きに
    float2 pulseOffset = float2(pulse * cos(time * 10.0), pulse * sin(time * 10.0));

    // 呼吸と心拍によるUV座標の最終補正
    uv += float2(0.0, offsetY) + pulseOffset;

    // テクスチャから色をサンプリング
    float4 color = texture_map.Sample(sampler_states[POINT], uv);

    // アルファ値を1.0に設定（常に不透明）
    color.a = 1.0;

    // 最終的な色を返す
    return color;
}