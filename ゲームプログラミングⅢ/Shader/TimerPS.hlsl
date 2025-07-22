#include "fullscreenquad.hlsli"

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps : register(t10);


float MaxTime = 180.0f; // 3��

// ��ʃT�C�Y�i���K��UV��0�`1�j
static const float2 DigitSize = float2(0.03, 0.05); // 1�����T�C�Y�i���A�����j
static const int NumDigits = 8; // mm:ss:ms ��8����
static const float2 DisplayPos = float2(0.5 - (DigitSize.x * NumDigits) / 2.0, 0.05); // ������


// 7�Z�O�����g�\���r�b�g�ia,b,c,d,e,f,g�j
// bit 0 = a ��
// bit 1 = b �E��
// bit 2 = c �E��
// bit 3 = d ��
// bit 4 = e ����
// bit 5 = f ����
// bit 6 = g ����

// ����0�`9��7�Z�O�����g�r�b�g�}�b�v
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

// �R�����̕`��i�㉺2�_�j
bool IsColonSegment(float2 p)
{
    // �����̏c������2�̏��~�i���΍��Wp��0�`1�̕��������W�j
    float2 upperDot = float2(0.5, 0.3);
    float2 lowerDot = float2(0.5, 0.7);
    float r = 0.07;
    float upperDist = length(p - upperDot);
    float lowerDist = length(p - lowerDot);
    return (upperDist < r || lowerDist < r);
}

// 7�Z�O�����g���̊e�Z�O�����g��`���֐�
bool IsSegmentOn(float2 uv, int segment)
{
    // uv �� 0�`1 �̕��������W (x�����Ay����)

    float thickness = 0.15; // �Z�O�����g�̑���

    switch (segment)
    {
        case 0: // a �㉡
            return (uv.y < thickness) && (uv.x > thickness) && (uv.x < 1 - thickness);
        case 1: // b �E��c
            return (uv.x > 1 - thickness) && (uv.y > thickness) && (uv.y < 0.5 - thickness / 2);
        case 2: // c �E���c
            return (uv.x > 1 - thickness) && (uv.y > 0.5 + thickness / 2) && (uv.y < 1 - thickness);
        case 3: // d ����
            return (uv.y > 1 - thickness) && (uv.x > thickness) && (uv.x < 1 - thickness);
        case 4: // e �����c
            return (uv.x < thickness) && (uv.y > 0.5 + thickness / 2) && (uv.y < 1 - thickness);
        case 5: // f ����c
            return (uv.x < thickness) && (uv.y > thickness) && (uv.y < 0.5 - thickness / 2);
        case 6: // g ������
            return (uv.y > 0.5 - thickness / 2) && (uv.y < 0.5 + thickness / 2) && (uv.x > thickness) && (uv.x < 1 - thickness);
        default:
            return false;
    }
}

// �w�肵�����������̃s�N�Z����`��
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

// �R�����`��
float DrawColon(float2 uv)
{
    return IsColonSegment(uv) ? 1.0 : 0.0;
}

// �e�����̎�ނ�enum��

#define CHAR_DIGIT 0
#define CHAR_COLON 1


// ����������֐��F�ʒu�ɉ����ĉ���`��������
float DrawChar(float2 uv, int index, int value)
{
    // uv: ��������0~1���W
    // index: 0~7 (mm:ss:ms)
    // value: 0~9�̐��� or -1�ŃR����

    if (value == -1)
        return DrawColon(uv);
    else
        return DrawDigit(uv, value);
}

float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD) : SV_Target
{
    // uv��0�`1�̉�ʍ��W

    // �\���J�n�ʒu���l��
    float2 posInDisplay = (uv - DisplayPos) / DigitSize;
    float4 background = texture_maps.Sample(sampler_states[0], uv);

    // �\���̈�O�͓���
    if (posInDisplay.x < 0 || posInDisplay.x > NumDigits || posInDisplay.y < 0 || posInDisplay.y > 1)
    {
        background.a = 1;
    // �̈�O�͔w�i�����̂܂ܕԂ�
        return background;
    }

    int charIndex = (int) posInDisplay.x;
    float2 charUV = float2(frac(posInDisplay.x), posInDisplay.y);

    // �c�莞�Ԃ���mm:ss:ms���v�Z
    float time = remTime; // �c�莞�ԁi�b�j
    //float time = 180 - iTime; // �����ł�iTime���g�p�i��Ƃ��āj
    //float RemainingTime = 180.0 - iTime;
    //float time = max(RemainingTime, 0.0);

    //float time = 123.45;
    if (time < 0)
        time = 0;
    int minutes = (int) (time / 60);
    int seconds = (int) (time) % 60;
    int milliseconds = (int) ((time - floor(time)) * 100);

    // 8�����ɕ���
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

    // �w�i���擾
    //float4 background = texture_maps.Sample(sampler_states[0], uv);

    // �����F�i���j
    float4 textColor = float4(0, 1, 0, 1);

    // �����Fcol �� 1 �̂Ƃ��͔��A0 �Ȃ�w�i
    float4 color = lerp(background, textColor, col);

    return color;
}