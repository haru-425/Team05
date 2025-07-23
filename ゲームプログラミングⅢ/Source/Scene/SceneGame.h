#pragma once
#include <memory>
#include "Stage.h"
#include "ICameraController.h"
#include "FPCameraController.h"
#include "FreeCameraController.h"
#include "LightDebugCameraController.h"
#include "Scene.h"
#include <memory>
#include "Player/Player.h"
#include"miniMap.h"
#include "3DAudio/3DAudio.h"
#include"Metar.h"

#include "System/ShadowCaster.h"

extern float reminingTime;
// �Q�[���V�[��
class SceneGame :public Scene
{
public:
	SceneGame() {}
	SceneGame(int Life):life_number(Life){}
	~SceneGame() override {}

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

private:
	void Collision();

	void PlayerVsStage();

	void PlayerVsEnemy();

	void PlayerVsDoor(); ///< �h�A�Ƃ̓����蔻��

	void UpdateCamera(float elapsedTime);

	void TutorialUpdate(float elapsedTime);

private:
	Stage* stage = nullptr;

	std::unique_ptr<ICameraController> i_CameraController = nullptr;
	std::shared_ptr<Player> player;
	std::shared_ptr<Enemy> enemy;
	std::shared_ptr<Metar> metar;
	MiniMap* minimap = nullptr;
	bool tutorial_Flug = false;
	int tutorial_Step = 0;
	float tutorialTimer = 0.0f;
	float button_effect;
	float timer = 0.0f; // �^�C�}�[
	float transTimer = 0.0f; // �V�[���J�ڃ^�C�}�[
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1�b��ɑJ�ڂ���V�[��
	enum class SelectTrans {
		Clear, // �V�[���N���A
		GameOver, // �Q�[���I�[�o�[
		cnt,
	};
	SelectTrans selectTrans = SelectTrans::GameOver; // �V�[���J�ڑI��
	// ==============================
	// �V���h�E�֘A�萔
	// ==============================

	// ���C�g���_����̃r���[�ˉe�s��i�V���h�E�}�b�v�쐬�p�j
	DirectX::XMFLOAT4X4 lightViewProjection;

	// �V���h�E�̐[�x�o�C�A�X�i�A�N�l�h�~�j
	float shadowBias = 0.001f;

	// �e�̐F�i��▾��߂̃O���[�j
	DirectX::XMFLOAT3 shadowColor = { 0.7812f,0.7812f,0.7812f };
	DirectX::XMFLOAT4 edgeColor = { .0f,.0f,.0f,1.0f };

	// �V���h�E�}�b�v�`��͈́i�f�o�b�O�`��p�H�j
	float SHADOWMAP_DRAWRECT = 30.0f;

	// �V���h�E�}�b�v�`��E������S������N���X
	std::unique_ptr<ShadowCaster> shadow;


	// ==============================
	// �t�H�O�E�����֘A�萔
	// ==============================

	// �A���r�G���g���C�g�̐F�i�Â߂̃O���[�j
	DirectX::XMFLOAT4 ambientColor = { 0.1093f, 0.1093f, 0.1093f, 1.0f };

	// �t�H�O�̐F�i���ɋ߂��O���[�j
	DirectX::XMFLOAT4 fogColor = { .0f,.0f,.0f, 1.0f };

	// �t�H�O�͈̔́i�J�n20.0�A�I��100.0�j
	DirectX::XMFLOAT4 fogRange = { 10.0f, 30.0f, 0, 0 };

	// ==============================
	// �J�����E���C�g����
	// ==============================

	// ���s�����̕����x�N�g���i�΂ߏォ��Ǝˁj
	DirectX::XMFLOAT3 lightDirection = { 0.0f, -3.0f, 0.0f };

	// �J�����̃��[���h���W
	DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f, 0.0f };
	void UpdateConstants(RenderContext& rc);


	// 3D�I�[�f�B�I�V�X�e��

	Audio3DSystem audioSystem; ///< 3D�I�[�f�B�I�V�X�e���̃C���X�^���X

	int life_number;

	// ======================================
	// �`���|�g���A���̃X�v���C�g�z��
	// ======================================
	std::unique_ptr<Sprite> tutorial[13];


};