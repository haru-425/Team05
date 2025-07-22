#include "fullscreenquad.hlsli"

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps : register(t10);


float MaxTime = 180.0f; // 3分

// 画面サイズ（正規化UVは0～1）
static const float2 DigitSize = float2(0.03, 0.05); // 1文字サイズ（幅、高さ）
static const int NumDigits = 8; // mm:ss:ms は8文字
static const float2 DisplayPos = float2(0.5 - (DigitSize.x * NumDigits) / 2.0, 0.05); // 中央寄せ


// 7セグメント構成ビット（a,b,c,d,e,f,g）
// bit 0 = a 上
// bit 1 = b 右上
// bit 2 = c 右下
// bit 3 = d 下
// bit 4 = e 左下
// bit 5 = f 左上
// bit 6 = g 中央

// 数字0～9の7セグメントビットマップ
static const uint Segments[10] =
{
    0x3F, // 0 = 0b0111111 a,b,c,d,e,f (g=0)
    0x06, // 1 = 0b0000110 b,c
    0x5B, // 2 = 0b1011011 a,b,d,e,g
    0x4F, // 3 = 0b1001111 a,b,c,d,g
    0x66, // 4 = 0b1100110 b,c,f,g
    0x6D, // 5 = 0b1101101 a,c,d,f,g
    0x7D, // 6 = 0b1111101 a,c,d,e,f,g
    0x07, // 7 = 0b0000111 a,b,c
    0x7F, // 8 = 0b1111111 a,b,c,d,e,f,g
    0x6F // 9 = 0b1101111 a,b,c,d,f,g
};

// コロンの描画（上下2点）
bool IsColonSegment(float2 p)
{
    // 中央の縦方向の2つの小円（相対座標pは0～1の文字内座標）
    float2 upperDot = float2(0.5, 0.3);
    float2 lowerDot = float2(0.5, 0.7);
    float r = 0.07;
    float upperDist = length(p - upperDot);
    float lowerDist = length(p - lowerDot);
    return (upperDist < r || lowerDist < r);
}

// 7セグメント中の各セグメントを描く関数
bool IsSegmentOn(float2 uv, int segment)
{
    // uv は 0～1 の文字内座標 (x横幅、y高さ)

    float thickness = 0.15; // セグメントの太さ

    switch (segment)
    {
        case 0: // a 上横
            return (uv.y < thickness) && (uv.x > thickness) && (uv.x < 1 - thickness);
        case 1: // b 右上縦
            return (uv.x > 1 - thickness) && (uv.y > thickness) && (uv.y < 0.5 - thickness / 2);
        case 2: // c 右下縦
            return (uv.x > 1 - thickness) && (uv.y > 0.5 + thickness / 2) && (uv.y < 1 - thickness);
        case 3: // d 下横
            return (uv.y > 1 - thickness) && (uv.x > thickness) && (uv.x < 1 - thickness);
        case 4: // e 左下縦
            return (uv.x < thickness) && (uv.y > 0.5 + thickness / 2) && (uv.y < 1 - thickness);
        case 5: // f 左上縦
            return (uv.x < thickness) && (uv.y > thickness) && (uv.y < 0.5 - thickness / 2);
        case 6: // g 中央横
            return (uv.y > 0.5 - thickness / 2) && (uv.y < 0.5 + thickness / 2) && (uv.x > thickness) && (uv.x < 1 - thickness);
        default:
            return false;
    }
}

// 指定した数字文字のピクセルを描画
float DrawDigit(float2 uv, int digit)
{
    if (digit < 0 || digit > 9)
        return 0;

    uint segBits = Segments[digit];
    float col = 0;
    [unroll]
    for (int i = 0; i < 7; i++)
    {
        if ((segBits & (1 << i)) != 0)
        {
            if (IsSegmentOn(uv, i))
                col = 1.0;
        }
    }
    return col;
}

// コロン描画
float DrawColon(float2 uv)
{
    return IsColonSegment(uv) ? 1.0 : 0.0;
}

// 各文字の種類をenumで

#define CHAR_DIGIT 0
#define CHAR_COLON 1


// 文字列を作る関数：位置に応じて何を描くか決定
float DrawChar(float2 uv, int index, int value)
{
    // uv: 文字内の0~1座標
    // index: 0~7 (mm:ss:ms)
    // value: 0~9の数字 or -1でコロン

    if (value == -1)
        return DrawColon(uv);
    else
        return DrawDigit(uv, value);
}

float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD) : SV_Target
{
    // uvは0～1の画面座標

    // 表示開始位置を考慮
    float2 posInDisplay = (uv - DisplayPos) / DigitSize;
    float4 background = texture_maps.Sample(sampler_states[0], uv);

    // 表示領域外は透明
    if (posInDisplay.x < 0 || posInDisplay.x > NumDigits || posInDisplay.y < 0 || posInDisplay.y > 1)
    {
        background.a = 1;
    // 領域外は背景をそのまま返す
        return background;
    }

    int charIndex = (int) posInDisplay.x;
    float2 charUV = float2(frac(posInDisplay.x), posInDisplay.y);

    // 残り時間からmm:ss:msを計算
    float time = remTime; // 残り時間（秒）
    //float time = 180 - iTime; // ここではiTimeを使用（例として）
    //float RemainingTime = 180.0 - iTime;
    //float time = max(RemainingTime, 0.0);

    //float time = 123.45;
    if (time < 0)
        time = 0;
    int minutes = (int) (time / 60);
    int seconds = (int) (time) % 60;
    int milliseconds = (int) ((time - floor(time)) * 100);

    // 8文字に分割
    // mm:ss:ms
    // index: 0 1 2 3 4 5 6 7
    // chars: m m : s s : m m

    int digitValue = 0;
    bool isColon = false;

    switch (charIndex)
    {
        case 0:
            digitValue = minutes / 10;
            break;
        case 1:
            digitValue = minutes % 10;
            break;
        case 2:
            isColon = true;
            break;
        case 3:
            digitValue = seconds / 10;
            break;
        case 4:
            digitValue = seconds % 10;
            break;
        case 5:
            isColon = true;
            break;
        case 6:
            digitValue = milliseconds / 10;
            break;
        case 7:
            digitValue = milliseconds % 10;
            break;
        default:
            return float4(0, 0, 0, 0);
    }

    float col = isColon ? DrawChar(charUV, charIndex, -1) : DrawChar(charUV, charIndex, digitValue);

    // 背景を取得
    //float4 background = texture_maps.Sample(sampler_states[0], uv);

    // 文字色（白）
    float4 textColor = float4(0, 1, 0, 1);

    // 合成：col が 1 のときは白、0 なら背景
    float4 color = lerp(background, textColor, col);

    return color;
}