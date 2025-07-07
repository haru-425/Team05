#pragma once

#include <vector>
#include <DirectXMath.h>

#include "System/RenderContext.h"
#include "System/ShapeRenderer.h"

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
	void RenderDebug(RenderContext& rc);

	void DebugGUI();

	void UpdateConstants(RenderContext& rc);

	void PointLightDebug(char* c,int begin,int end);
	void LineLightDebug(char* c, int begin,int end);

	const std::vector<PointLightConstants>& GetPointLights() const { return pointLights; }
	const std::vector<LineLightConstants>& GetLineLights() const { return lineLights; }


private:
	// �������
	std::vector<PointLightConstants> pointLights;
	std::vector<LineLightConstants>  lineLights;
	std::vector<LightData>           lightData;;

	// �S�̂̃��C�g���x�i��Z�W���j
	float lightPower = 5.0f;

	struct LightColors
	{
		DirectX::XMFLOAT4 red    = { 1,0,0,1 };
		DirectX::XMFLOAT4 blue   = { 0,0,1,1 };
		DirectX::XMFLOAT4 yellow = { 1,1,0,1 };
		DirectX::XMFLOAT4 green  = { 0,1,0,1 };
		DirectX::XMFLOAT4 purple = { 1,0,1,1 };
	};
	LightColors lightColor;

#define POINTLIGHT_MAX     47
#define LINELIGHT_MAX      42

#define POINTLIGHT_S_RANGE  6
#define POINTLIGHT_L_RANGE  8
#define LINELIGHT_RANGE     5
#define CEILNG_HEIGHT       3

};

