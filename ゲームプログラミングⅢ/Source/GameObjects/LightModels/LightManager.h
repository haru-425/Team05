#pragma once

#include <vector>
#include <DirectXMath.h>

#include "System/RenderContext.h"
#include "System/ShapeRenderer.h"
#include "System/ModelRenderer.h"

#include "./LightModels/LightPoint/LightPoint.h"
#include "./LightModels/LightBar/LightBar.h"
#include "./LightModels/LightTorus/LightTorus.h"

class LightManager
{
public:
	// �C���X�^���X�擾
	static LightManager& Instance()
	{
		static LightManager instance;
		return instance;
	}

private:
	// �_�����\���́i�ʒu�E�F�E�͈́j
	struct PointLightConstants
	{
		DirectX::XMFLOAT4 position;  // ���[���h���W�iw���g�p�j
		DirectX::XMFLOAT4 color;     // ���̐F
		float range;                 // �L�������i�����I�������j
	};
	struct TorusLightConstants
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 direction;
		DirectX::XMFLOAT4 color;
		float majorRadius;   // �h�[�i�c�̒��S�~�̔��a
		float minorRadius;   // �h�[�i�c�̑����i�f�ʉ~�̔��a�j
		float range;
	};

	// �������\���́i�n�_�E�I�_�E�F�E�͈́j
	struct LineLightConstants
	{
		DirectX::XMFLOAT4 start;     // ���[���h��Ԃł̎n�_
		DirectX::XMFLOAT4 end;       // ���[���h��Ԃł̏I�_
		DirectX::XMFLOAT4 color;     // ���̐F
		float range;                 // �L�������i���Ƃ̋����Ō����j
	};

	// �����ʒu�E�������i�[����ėp�f�[�^�i�J�X�^���p�r�Ȃǁj
	struct LightData
	{
		DirectX::XMFLOAT3 position;  // ���C�g�̈ʒu
		float             angle;     // �������]�i���g�p�Ȃ�[���j
		float             length;    // �����̒���
	};

public:
	LightManager() = default;
	~LightManager() = default;

	void Initialize();
	void Update();

	void Render(RenderContext& rc);
	void RenderDebugPrimitive(RenderContext& rc);

	void DebugGUI();

	void UpdateConstants(RenderContext& rc);

	void PointLightDebug(char* c,int begin,int end);
	void TorusLightDebug(char* c, int begin,int end);
	void LineLightDebug(char* c, int begin,int end);

	const std::vector<PointLightConstants>& GetPointLights() const { return pointLights; }
	const std::vector<LineLightConstants>& GetLineLights() const { return lineLights; }


private:
	// �������
	std::vector<PointLightConstants> pointLights;
	std::vector<TorusLightConstants> torusLights;
	std::vector<LineLightConstants>  lineLights;
	std::vector<LightData>           lightData;

	// ���f��
	std::vector<std::unique_ptr<LightPoint>>  pointLightsModel;
	std::vector<std::unique_ptr<LightTorus>>  torusLightsModel;
	std::vector<std::unique_ptr<LightBar>>    lineLightsModel;

	// �S�̂̃��C�g���x�i��Z�W���j
	float lightPower = 5.0f;

	struct LightColors
	{
		DirectX::XMFLOAT4 red    = { 0.5f,    0.0f   , 0.0f    ,1 };
		DirectX::XMFLOAT4 blue   = { 0.0f,    0.0664f, 0.3242f ,1 };
		DirectX::XMFLOAT4 yellow = { 1.0f,    0.9453f, 0.0f    ,1 };
		DirectX::XMFLOAT4 green  = { 0.2578f ,0.2578f, 0.0f    ,1 };
		DirectX::XMFLOAT4 purple = { 0.3242f ,0.0f   , 0.4492f ,1 };
	};
	LightColors lightColor;

#define POINTLIGHT_MAX     34
#define TORUSLIGHT_MAX     13
#define LINELIGHT_MAX      45

#define POINTLIGHT_RANGE    5.5f

#define TORUSLIGHT_RANGE    6
#define TORUSLIGHT_MAJOR    5.5
#define TORUSLIGHT_MINOR    0.2

#define LINELIGHT_RANGE     5

#define CEILNG_HEIGHT       2.999f

	std::unique_ptr<Model> models[3];
	std::unique_ptr<LoadTextures> textures[3];
};

