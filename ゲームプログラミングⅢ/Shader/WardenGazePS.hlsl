#include "FullScreenQuad.hlsli"

/// @file WardenGazePS.hlsl
/// @brief 視線の揺らぎや探索効果を表現するポストプロセス用ピクセルシェーダー
/// @details 疑似ランダムや時間変化による視線の動き、テクスチャサンプリングを行う。
///          サンプラーやテクスチャ、各種ユーティリティ関数を定義。

//----------------------------------------
// 定数定義
//----------------------------------------

/// @brief 色収差の強度係数
#define CA_AMT 1.01

/// @brief サンプラーの種類: Pointサンプリング
#define POINT 0
/// @brief サンプラーの種類: Linearサンプリング
#define LINEAR 1
/// @brief サンプラーの種類: Anisotropicサンプリング
#define ANISOTROPIC 2

//----------------------------------------
// サンプラーステート・テクスチャ
//----------------------------------------

/// @brief サンプラーステート配列
/// @details POINT, LINEAR, ANISOTROPIC の3種類のサンプリング方式を格納
SamplerState sampler_states[3] : register(s0);

/// @brief 入力テクスチャ
Texture2D texture_map : register(t10);

//----------------------------------------
// 疑似ランダム生成関数
//----------------------------------------

/// @brief 疑似ランダム値を生成する関数
/// @param seed ランダムシードとなる値
/// @return 0.0～1.0の範囲の疑似ランダム値
float rand(float seed)
{
    return frac(sin(seed * 12.9898) * 43758.5453);
}

/// @brief 2次元の疑似ランダム値を生成する関数
/// @param seed ランダムシードとなる値
/// @return float2型の疑似ランダム値（各成分0.0～1.0）
float2 rand2(float seed)
{
    return float2(rand(seed), rand(seed + 42.0));
}

//----------------------------------------
// ランダムパラメータ生成
//----------------------------------------

/// @brief X軸・Y軸それぞれでランダムな振幅・周期パラメータを生成
/// @param[in]  t        現在の時間
/// @param[out] amplitude X・Y軸ごとの振幅（揺れ幅）
/// @param[out] period    X・Y軸ごとの周期（揺れる速さ）
/// @details 時間経過に応じて滑らかにパラメータが変化するよう補間する
void GetRandomParams2(float t, out float2 amplitude, out float2 period)
{
    float interval = 2.0; ///< パラメータが切り替わる間隔（秒）

    float currentIndex = floor(t / interval); ///< 現在のインデックス
    float progress = frac(t / interval); ///< インデックス内での進行度（0.0～1.0）

    // X軸用とY軸用で異なるシード値を使用
    float2 prevX = rand2(currentIndex);
    float2 nextX = rand2(currentIndex + 1.0);
    float2 prevY = rand2(currentIndex + 100.0); // シードをずらすことで独立性を確保
    float2 nextY = rand2(currentIndex + 101.0);

    // X軸の振幅・周期（前後の値を補間）
    float prevAmpX = lerp(0.04, 0.06, prevX.x);
    float nextAmpX = lerp(0.04, 0.06, nextX.x);
    float prevPeriodX = lerp(4.0, 4.0, prevX.y);
    float nextPeriodX = lerp(4.0, 4.0, nextX.y);

    // Y軸の振幅・周期（前後の値を補間）
    float prevAmpY = lerp(0.002, 0.01, prevY.x);
    float nextAmpY = lerp(0.002, 0.01, nextY.x);
    float prevPeriodY = lerp(3.0, 4.0, prevY.y);
    float nextPeriodY = lerp(3.0, 4.0, nextY.y);

    // スムーズに値を補間して出力
    amplitude.x = lerp(prevAmpX, nextAmpX, smoothstep(0.0, 1.0, progress));
    amplitude.y = lerp(prevAmpY, nextAmpY, smoothstep(0.0, 1.0, progress));
    period.x = lerp(prevPeriodX, nextPeriodX, smoothstep(0.0, 1.0, progress));
    period.y = lerp(prevPeriodY, nextPeriodY, smoothstep(0.0, 1.0, progress));
}

//----------------------------------------
// 視線の揺らぎオフセット計算
//----------------------------------------

/// @brief 時間に応じた視線の揺らぎオフセットを計算
/// @param t 現在の時間
/// @return float2型のオフセット値（UV座標に加算して使用）
/// @details X軸・Y軸ごとに異なる周期・振幅で揺らぎを生成
float easeInOutElastic(float x)
{
    const float c5 = (2.0 * 3.14159) / 4.5;

    if (x < 0.5)
    {
        return -(pow(2.0, 20.0 * x - 10.0) * sin((20.0 * x - 11.125) * c5)) * 0.5;
    }
    else
    {
        return (pow(2.0, -20.0 * x + 10.0) * sin((20.0 * x - 11.125) * c5)) * 0.5 + 1.0;
    }
}

// PingPong型の進行度（0→1→0→1…）
float pingpong(float x)
{
    return 1.0 - abs(1.0 - frac(x) * 2.0);
}

float2 GetEyeOffset(float t)
{
    float2 amplitude, period;
    GetRandomParams2(t, amplitude, period);

    // ピンポン進行度を生成（位相ずらし含む）
    float progressX = pingpong(t / period.x);
    float progressY = pingpong(t / period.y + 0.3); // Y軸はずらして自然さを出す

    // Elasticイージングを適用し [-1,1] の範囲に変換
    float x = (easeInOutElastic(progressX) * 2.0 - 1.0) * amplitude.x;
    float y = (easeInOutElastic(progressY) * 2.0 - 1.0) * amplitude.y;

    return float2(x, y);
}
//----------------------------------------
// メインピクセルシェーダー
//----------------------------------------

/// @brief ポストプロセス用メインピクセルシェーダー
/// @param pin 頂点シェーダーから渡される構造体（UV座標などを含む）
/// @return 出力カラー（float4）
/// @details 時間に応じて視線が揺らぐようにUV座標を変化させ、テクスチャをサンプリングして出力
float4 main(VS_OUT pin) : SV_Target
{
    //--- 視線の揺らぎオフセットを計算 ---
    float2 offset = GetEyeOffset(iTime);

    //--- UV座標にオフセットを加算 ---
    float2 new_uv = pin.texcoord + offset;

    //--- テクスチャをサンプリング ---
    float4 color = texture_map.Sample(sampler_states[LINEAR], new_uv);

    //--- 最終カラーを出力 ---
    return color;
}
