/**
 * @file BlurPS.hlsl
 * @brief ガウスぼかしとブルーム効果を適用するピクセルシェーダ
 * 
 * このシェーダは、入力テクスチャに対してガウスぼかしを行い、ブルーム効果を加えた後、
 * トーンマッピング（HDR→SDR）を適用して最終的な色を出力します。
 * 
 * @see https://en.wikipedia.org/wiki/Gaussian_blur
 * @see https://en.wikipedia.org/wiki/Bloom_(shader_effect)
 */

#include "fullscreenQuad.hlsli"

/// サンプラーステートのインデックス定義
#define POINT 0         ///< 最近傍補間サンプラー
#define LINEAR 1        ///< 線形補間サンプラー
#define ANISOTROPIC 2   ///< 異方性フィルタリングサンプラー

/// サンプラーステート配列。3種類のサンプリング方法を格納。
SamplerState sampler_states[3] : register(s0);

/// テクスチャ配列。0:元画像, 1:ぼかし対象, 2,3:未使用または拡張用。
Texture2D texture_maps[4] : register(t10);

/**
 * @brief ピクセルシェーダのメイン関数
 * 
 * 入力されたテクスチャ座標に基づき、ガウスぼかしとブルーム効果を適用した色を出力します。
 * 
 * @param[in] pin 頂点シェーダから渡される構造体。テクスチャ座標などを含む。
 * @return float4 最終的な色（RGBA）
 */
float4 main(VS_OUT pin) : SV_TARGET
{
    /// テクスチャのミップレベル（通常0）、幅、高さ、ミップレベル数を格納する変数
    uint mip_level = 0, width, height, number_of_levels;
    texture_maps[1].GetDimensions(mip_level, width, height, number_of_levels);

    /// 元画像の色を異方性サンプラーで取得
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    /// 元画像のアルファ値を保持
    float alpha = color.a;

    /// ぼかし後の色を格納する変数（初期値0）
    float3 blur_color = 0;

    /// ガウスカーネルの合計値（正規化用）
    float gaussian_kernel_total = 0;

    // --- ガウスぼかしのパラメータ定義 ---
    /**
     * @brief ガウスカーネルの半径
     * 
     * 例: 3の場合、カーネルサイズは7x7（-3～+3）となる。
     * 値を大きくするとぼかし範囲が広がる。
     */
    const int gaussian_half_kernel_size = 10;

    /**
     * @brief ガウス分布の標準偏差
     * 
     * 値を大きくするとぼかしがなだらかに、値を小さくするとシャープになる。
     */
    const float gaussian_sigma = 1.0;

    /**
     * @brief ブルーム効果の強度
     * 
     * ぼかし成分をどれだけ元画像に加算するかを決定する。
     */
    const float bloom_intensity = 2.0;

    // --- ガウスぼかし処理 ---
    /**
     * @details
     * テクスチャ座標周辺のピクセルをガウス分布に基づいてサンプリングし、加重平均を計算する。
     * これにより、画像全体が柔らかくぼける。
     * 
     * @see https://en.wikipedia.org/wiki/Gaussian_blur
     */
    [unroll]
    for (int x = -gaussian_half_kernel_size; x <= +gaussian_half_kernel_size; x += 1)
    {
        [unroll]
        for (int y = -gaussian_half_kernel_size; y <= +gaussian_half_kernel_size; y += 1)
        {
            /// ガウスカーネルの重みを計算
            float gaussian_kernel = exp(-(x * x + y * y) / (2.0 * gaussian_sigma * gaussian_sigma)) /
                  (2 * 3.14159265358979 * gaussian_sigma * gaussian_sigma);

            /// 周辺ピクセルを線形補間サンプラーで取得し、ガウス重みを掛けて加算
            blur_color += texture_maps[1].Sample(
                sampler_states[LINEAR],
                pin.texcoord + float2(x * 1.0 / width, y * 1.0 / height)
            ).rgb * gaussian_kernel;

            /// カーネル合計値を更新（正規化用）
            gaussian_kernel_total += gaussian_kernel;
        }
    }

    /// ぼかし色をカーネル合計値で正規化
    blur_color /= gaussian_kernel_total;

    /// 元画像にブルーム（ぼかし）成分を加算
    color.rgb += blur_color * bloom_intensity;

#if 1
    // --- トーンマッピング処理（HDR→SDR） ---
    /**
     * @brief トーンマッピング
     * 
     * HDR画像をSDRに変換し、明るさを調整する。
     * exposure値を大きくすると全体的に明るくなる。
     * 
     * @see https://en.wikipedia.org/wiki/Tone_mapping
     */
    const float exposure = 1.2;
    color.rgb = 1 - exp(-color.rgb * exposure);
#endif

#if 0
    // --- ガンマ補正処理 ---
    /**
     * @brief ガンマ補正
     * 
     * ディスプレイ表示用に色を調整する。通常2.2が一般的。
     * 
     * @see https://en.wikipedia.org/wiki/Gamma_correction
     */
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, 1.0 / GAMMA);
#endif

    /// 最終的な色（RGB）と元画像のアルファ値を返す
    return float4(color.rgb, alpha);
}