cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	float4				lightDirection;
    float4              cameraPosition;
};

cbuffer CbFog : register(b2)
{
    float4 ambientColor;
    float4 fogColor;
    float4 fogRange;
};

cbuffer CbShadowmap : register(b3)
{
    row_major float4x4 lightViewProjection;    // ���C�g�̈ʒu���猩���ˉe�s��
    float3             shadowColor;            // �e�F
    float              shadowBias;             // �[�x�o�C�A�X
};

struct PointLight
{
    float4 position;
    float4 color;
    float range;
    float3 dummy;
};
struct LineLight
{
    float4 start;
    float4 end;
    float4 color;
    float range;
    float3 dummy;
};

cbuffer CbLights : register(b4)
{
    PointLight pointLights[47];
    LineLight lineLights[42];
    float power;
}