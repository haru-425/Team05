#pragma once
#include <memory>
#include "Stage.h"
#include "ICameraController.h"
#include "FPCameraController.h"
#include "FreeCameraController.h"
#include "Scene.h"
#include <memory>
#include "Player/Player.h"
#include "System/ShadowCaster.h"

// �Q�[���V�[��
class SceneGraphics :public Scene
{
public:
	SceneGraphics() {}
	~SceneGraphics() override {}

	// ���C�g�֘A�̏�����
	void SetPointLightData();

	// ������
	void Initialize()override;

	// �I����
	void Finalize()override;

	// �X�V����
	void Update(float elapsedTime)override;

	// �`�揈��
	void Render()override;

	// GUI�`��
	void DrawGUI()override;

	// �萔�̍X�V
	void UpdateConstants(RenderContext& rc);

private:
	Stage* stage = nullptr;

	std::unique_ptr<ICameraController> i_CameraController = nullptr;
	std::shared_ptr<Player> player;

private:
	// ==============================
	// �V���h�E�֘A�萔
	// ==============================

	// ���C�g���_����̃r���[�ˉe�s��i�V���h�E�}�b�v�쐬�p�j
	DirectX::XMFLOAT4X4 lightViewProjection;

	// �V���h�E�̐[�x�o�C�A�X�i�A�N�l�h�~�j
	float shadowBias = 0.001f;

	// �e�̐F�i��▾��߂̃O���[�j
	DirectX::XMFLOAT3 shadowColor = { 0.89f, 0.89f, 0.89f };

	// �V���h�E�}�b�v�`��͈́i�f�o�b�O�`��p�H�j
	float SHADOWMAP_DRAWRECT = 30.0f;

	// �V���h�E�}�b�v�`��E������S������N���X
	std::unique_ptr<ShadowCaster> shadow;


	// ==============================
	// �t�H�O�E�����֘A�萔
	// ==============================

	// �A���r�G���g���C�g�̐F�i�Â߂̃O���[�j
	DirectX::XMFLOAT4 ambientColor = { 0.25f, 0.273f, 0.335f, 1.0f };

	// �t�H�O�̐F�i���ɋ߂��O���[�j
	DirectX::XMFLOAT4 fogColor = { .0f,.0f,.0f, 1.0f };

	// �t�H�O�͈̔́i�J�n20.0�A�I��100.0�j
	DirectX::XMFLOAT4 fogRange = { 10.0f, 50.0f, 0, 0 };


	// ==============================
	// �J�����E���C�g����
	// ==============================

	// ���s�����̕����x�N�g���i�΂ߏォ��Ǝˁj
	DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 1.0f };

	// �J�����̃��[���h���W
	DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f, 0.0f };


	// ==============================
	// �_�����E�������̐ݒ�
	// ==============================

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
		DirectX::XMFLOAT3 dummy = { 0,0,0 };  // 16�o�C�g�A���C�������g�����p
	};

	// �����ʒu�E�������i�[����ėp�f�[�^�i�J�X�^���p�r�Ȃǁj
	struct LightData
	{
		DirectX::XMFLOAT3 position;  // ���C�g�̈ʒu
		DirectX::XMFLOAT3 angle;     // �������]�i���g�p�Ȃ�[���j
		float             length;    // �����̒���
	};

	// �������
	LightData lightData[8] = {};              // �ėp�����f�[�^�i�������ς݁j
	PointLightConstants pointLights[8];       // �ő�8�̓_����
	LineLightConstants lineLights[8];         // �ő�8�̐�����

	// �S�̂̃��C�g���x�i��Z�W���j
	float lightPower = 5.0f;
};
