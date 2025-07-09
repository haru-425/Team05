#include "FullScreenQuad.hlsli"

Texture2D sceneTex : register(t10); ///< シーンのテクスチャ
SamplerState samp : register(s0); ///< サンプラーステート

/**
 * @brief ちらつきパラメータ用定数バッファ
 */
cbuffer FlickerParams : register(b13)
{
    float flickerSpeed; ///< ちらつき中のノイズ速度（例：40.0）
    float flickerStrength; ///< 明るさの揺らぎの強さ（例：0.4）
    float flickerDuration; ///< ちらつき状態が続く時間（秒）（例：1.0）
    float flickerChance; ///< ちらつきが起こる確率（例：0.05）
}

/**
 * @brief 擬似ランダム関数
 * @param x 入力値（時間やシードなど）
 * @return 0～1の疑似乱数値
 */
float rand(float x)
{
    return frac(sin(x * 12.9898) * 43758.5453);
}

/**
 * @brief 線形補間付き滑らかなノイズ生成関数
 * @param time 現在時間
 * @param speed ノイズ変化速度
 * @return 0～1の滑らかなノイズ値
 */
float smoothNoise(float time, float speed)
{
    float base = floor(time * speed);
    float t = frac(time * speed);
    float a = rand(base);
    float b = rand(base + 1.0);
    return lerp(a, b, t);
}

/**
 * @brief ちらつき状態かどうかを判定
 * @param time 現在時間
 * @param duration ちらつき持続時間（秒）
 * @param chance ちらつき発生確率（0～1）
 * @return trueなら現在ちらつき状態
 */
bool isFlickering(float time, float duration, float chance)
{
    float group = floor(time / duration);
    return rand(group) < chance;
}

/**
 * @brief ちらつきパターンを乱数で選択（0～15）
 * @param seed シード値（例：ちらつきグループ時間など）
 * @return パターンID（0～15）
 */
int choosePattern(float seed)
{
    return (int) (rand(seed) * 16.0);
}

/**
 * @brief フェードイン・アウト用の三角波計算
 * @param t 0～1の時間パラメータ
 * @return 0～1のフェード値（三角波）
 */
float fadeInOut(float t)
{
    return 1.0 - abs(frac(t) * 2.0 - 1.0);
}

/**
 * @brief パターンごとの明るさ（輝度）計算
 * @param time 現在時間
 * @param pattern パターンID（0～15）
 * @param flickerSpeed ノイズ速度パラメータ
 * @param flickerStrength 揺らぎ強度パラメータ
 * @return 0～1の明度値
 */
float getBrightness(float time, int pattern, float flickerSpeed, float flickerStrength)
{
    switch (pattern)
    {
        case 0:
            // 高速ノイズによる微細な明度揺らぎ
            return 1.0 - smoothNoise(time, flickerSpeed) * flickerStrength;

        case 1:
            // 一瞬完全消灯（瞬間的に真っ暗になる）
            return fmod(time * 10.0, 1.0) < 0.1 ? 0.0 : 1.0;

        case 2:
        {
            // 連続点滅（長めの暗と明が繰り返す）
                float t = fmod(time * 5.0, 1.0);
                return (t < 0.3 || (t > 0.5 && t < 0.6)) ? 0.3 : 1.0;
            }

        case 3:
            // ランダムに明度がジャンプする不規則ちらつき
            return lerp(1.0, 0.5, rand(floor(time * 20.0) * 17.0));

        case 4:
        {
            // ゆっくりフェードイン・アウトする穏やかな揺らぎ
                float t = frac(time * 1.5);
                return lerp(0.7, 1.0, abs(sin(t * 3.14159)));
            }

        case 5:
        {
            // 複数の速いフラッシュを重ねた複雑なちらつき
                float basePulse = smoothNoise(time * 20.0, 40.0);
                float flickerPulse = smoothNoise(time * 60.0, 80.0);
                float flicker = lerp(basePulse, flickerPulse, 0.5);
                return 1.0 - flicker * flickerStrength * 1.5;
            }

        case 6:
        {
            // 不規則な点滅（時間にノイズを加えてリズムを崩す）
                float noiseTime = time + rand(floor(time * 3.0) * 7.0);
                float t = fmod(noiseTime * 6.0, 1.0);
                return (t < 0.25) ? 0.0 : 1.0;
            }

        case 7:
        {
            // 緩やかな揺らぎに瞬間的な暗転を重ねた複合パターン
                float base = lerp(0.8, 1.0, smoothNoise(time, 2.0));
                float flash = fmod(time * 12.0, 1.0) < 0.05 ? 0.0 : 1.0;
                return base * flash;
            }

        case 8:
        {
            // 徐々に明るくなり、急に暗転するじんわり点灯パターン
                float t = frac(time * 2.0);
                return (t < 0.8) ? lerp(0.5, 1.0, t / 0.8) : 0.0;
            }

        case 9:
        {
            // 短い間隔の高速点滅（パチパチとした明暗を繰り返す）
                float t = fmod(time * 30.0, 1.0);
                return t < 0.15 ? 0.0 : 1.0;
            }

        case 10:
        {
            // 徐々に明るさが落ちて一瞬暗転、繰り返し強めのちらつき
                float t = frac(time * 1.2);
                if (t < 0.9)
                    return lerp(1.0, 0.2, t / 0.9);
                else
                    return 0.0;
            }

        case 11:
        {
            // ノイズと周期点滅を組み合わせた不安定なちらつき
                float base = 1.0 - smoothNoise(time, flickerSpeed) * flickerStrength * 0.6;
                float pulse = (fmod(time * 7.0, 1.0) < 0.4) ? 0.6 : 1.0;
                return base * pulse;
            }

        case 12:
        {
            // 低周波ゆらぎ＋ランダム一瞬暗転を組み合わせたパターン
                float slowNoise = lerp(0.85, 1.0, smoothNoise(time, 0.5));
                float flash = rand(floor(time * 4.0) * 19.0) < 0.1 ? 0.0 : 1.0;
                return slowNoise * flash;
            }

        case 13:
        {
            // 急激な点滅とゆるいフェードの混合パターン
                float pulse = fmod(time * 15.0, 1.0);
                float fade = frac(time * 2.0);
                float base = (pulse < 0.1) ? 0.0 : lerp(0.7, 1.0, fade);
                return base;
            }

        case 14:
        {
            // 突然の暗転が不規則に発生するパターン
                float noise = rand(floor(time * 10.0) * 13.0);
                return noise < 0.15 ? 0.0 : 1.0;
            }

        case 15:
        {
            // 緩やかな明度上昇と突発的な小さなフラッシュを混ぜたパターン
                float base = lerp(0.6, 1.0, frac(time * 0.8));
                float flash = fmod(time * 40.0, 1.0) < 0.03 ? 1.2 : 1.0;
                return saturate(base * flash);
            }

        default:
            // 安全策として通常明るさ
            return 1.0;
    }
}

/**
 * @brief ピクセルシェーダーメイン関数
 * @param input 頂点シェーダーからの入力（UV座標含む）
 * @return ちらつきを加えた最終色
 */
float4 main(VS_OUT input) : SV_Target
{
    float2 uv = input.texcoord;

    // 元シーンテクスチャ色取得
    float4 color = sceneTex.Sample(samp, uv);

    float brightness = 1.0;

    // 現在のちらつきグループ時間（ちらつき持続時間単位で区切る）
    float groupTime = floor(iTime / flickerDuration);

    // ちらつき状態ならパターン選択して明度計算
    if (isFlickering(iTime, flickerDuration, flickerChance))
    {
        int pattern = choosePattern(groupTime);
        float flickerTime = frac(iTime / flickerDuration);
        float fade = fadeInOut(flickerTime);

        float baseBrightness = getBrightness(iTime, pattern, flickerSpeed, flickerStrength);

        // フェードインアウトで明度を滑らかに変化させる
        brightness = lerp(1.0, baseBrightness, fade);
    }

    // 最終的にシーン色に明度を乗算
    color.rgb *= brightness;

    return color;
}



//#include "FullScreenQuad.hlsli"
//
//Texture2D sceneTex : register(t10);
//SamplerState samp : register(s0);
//
//cbuffer FlickerParams : register(b13)
//{
//    float flickerSpeed; // ちらつき中のノイズ速度（例：40.0）
//    float flickerStrength; // 明るさの揺らぎの強さ（例：0.4）
//    float flickerDuration; // ちらつき状態が続く時間（秒）（例：1.0）
//    float flickerChance; // ちらつきが起こる確率（例：0.05）
//};
//
//
//// 擬似ランダム
//float rand(float x)
//{
//    return frac(sin(x * 12.9898) * 43758.5453);
//}
//
//// ノイズによるちらつき（高周波の明るさゆらぎ）
//float getFlickerNoise(float time, float speed)
//{
//    float base = floor(time * speed);
//    float t = frac(time * speed);
//    float a = rand(base);
//    float b = rand(base + 1.0);
//    return lerp(a, b, t);
//}
//
//// "ちらつき状態に入っているかどうか" を判定
//bool isInFlickerState(float time, float duration, float chance)
//{
//    float groupTime = floor(time / duration); // ちらつき候補ブロック
//    float r = rand(groupTime);
//    return r < chance;
//}
//
//float4 main(VS_OUT input) : SV_Target
//{
//    float2 uv = input.texcoord;
//    float4 color = sceneTex.Sample(samp, uv);
//
//    float brightness = 1.0;
//
//    // 一定確率でちらつき状態になる
//    if (isInFlickerState(iTime, flickerDuration, flickerChance))
//    {
//        float noise = getFlickerNoise(iTime, flickerSpeed);
//        brightness = 1.0 - (noise * flickerStrength);
//    }
//
//    color.rgb *= brightness;
//    return color;
//}
