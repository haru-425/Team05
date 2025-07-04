#include "FullScreenQuad.hlsli"
#define CURVATURE 4.2
#define BLUR .021
//#define CA_AMT 1.024
#define CA_AMT 1.01
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_map : register(t10);

/**
 * @brief CRTディスプレイ風の歪み・色収差・走査線・エッジフェード効果を付与するピクセルシェーダ
 *
 * このシェーダは、UV座標の湾曲、エッジのフェードアウト、色収差（RGBずらし）、
 * 走査線やマスクによるCRTディスプレイ特有の表現を合成し、
 * レトロな映像表現を実現します。
 *
 * @param pin 頂点シェーダから渡される出力構造体。テクスチャ座標(texcoord)やピクセル位置(position)を含みます。
 * @return float4 最終的な色。アルファ値は常に1.0です。
 */
float4 main(VS_OUT pin) : SV_Target
{
    // テクスチャ座標（0～1の範囲）
    float2 uv = pin.texcoord;

    // --- CRT湾曲効果 ---
    // UV座標を-1～1に変換し、YとXを入れ替えて歪み量を計算
    float2 crtUV = uv * 2.0 - 1.0;
    float2 offset = crtUV.yx / CURVATURE;
    // オフセットの2乗を掛けて非線形な湾曲を加える
    crtUV += crtUV * offset * offset;
    // 元のUV空間（0～1）に戻す
    crtUV = crtUV * 0.5 + 0.5;

    // --- エッジフェード効果 ---
    // 画面端で徐々に透明になるようにエッジの強度を計算
    float2 edge = smoothstep(0.0, BLUR, crtUV) * (1.0 - smoothstep(1.0 - BLUR, 1.0, crtUV));

    // --- 色収差（クロマチックアバレーション） ---
    // R,G,B各成分をわずかにずらしてサンプリングし、色ズレを表現
    float3 col;
    // 赤成分は左方向にずらしてサンプリング
    col.r = texture_map.Sample(sampler_states[2], (crtUV - 0.5) * CA_AMT + 0.5).r;
    // 緑成分はそのままサンプリング
    col.g = texture_map.Sample(sampler_states[2], crtUV).g;
    // 青成分は右方向にずらしてサンプリング
    col.b = texture_map.Sample(sampler_states[2], (crtUV - 0.5) / CA_AMT + 0.5).b;

    // エッジフェードをRGB全体に適用
    col *= edge.x * edge.y;

    // --- 走査線・マスク効果 ---
    // Y座標で2ピクセルごとに明るさを下げて走査線を表現
    // X座標で3ピクセルごとに明るさを下げてマスク効果を追加
    if (fmod(pin.position.y, 2.0) < 1.0)
        col *= 0.7;
    else if (fmod(pin.position.x, 3.0) < 1.0)
        col *= 0.7;
    else
        col *= 1.2;

    // 最終的な色を返す（アルファ値は常に1.0）
    return float4(col, 1.0);
}