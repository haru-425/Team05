#pragma once

#include "System/Sprite.h"
#include "Scene.h"
#include "System/Model.h"
#include "System/ModelRenderer.h"
#include <memory>
#include "System/ShadowCaster.h"
#include "Camera/CameraController/ICameraController.h"
#include "Camera/CameraController/FreeCameraController.h"
#include "Stage.h"

//�^�C�g���V�[��
class SceneTitle :public Scene
{
public:
	SceneTitle() {}
	~SceneTitle()override {}

	//������
	void Initialize()override;

	//�I����
	void Finalize()override;

	//�X�V����
	void Update(float elapsedTime)override;

	//�`�揈��
	void Render()override;

	//GUI�`��
	void DrawGUI()override;

private:
	void UpdateTransform();

	void UpdateConstants(RenderContext& rc);

private:
	Sprite* sprite = nullptr;
	float TitleTimer = 0.0f; // �^�C�g����ʂ̃^�C�}�[
	float TitleSignalTimer = 0.0f; // �^�C�g����ʂ̐M���^�C�}�[
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1�b��ɑJ�ڂ���V�[��

	std::unique_ptr<Stage> model;

	DirectX::XMFLOAT3 position = {0,0,0};
	DirectX::XMFLOAT3 scale = {1,1,1};
	DirectX::XMFLOAT3 angle = {0,0,0};
	DirectX::XMFLOAT4X4 world;

	std::unique_ptr<ICameraController> i_cameraController;

	/// �V�F�[�_�֘A
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


	float GraphicsScenetime = 0; //game�^�C�}�[
};
