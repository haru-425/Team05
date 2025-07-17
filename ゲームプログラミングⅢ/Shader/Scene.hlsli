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
    float4             edgeColor;
};

struct PointLight
{
    float4 position;
    float4 color;
    float range;
    float3 dummy;
};
struct TorusLight
{
    float4 position;
    float4 direction;
    float4 color;
    float range;
    float majorRadius;
    float minorRadius;
    float dummy;
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
    PointLight pointLights[256];
    TorusLight torusLights[256];
    LineLight lineLights[256];
    float power;
    float3 dummy;
}