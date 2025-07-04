#pragma once
#include <memory>
#include "Stage.h"
#include "ICameraController.h"
#include "FPCameraController.h"
#include "FreeCameraController.h"
#include "Scene.h"
#include <memory>
#include "Player/Player.h"
#include"miniMap.h"

// �Q�[���V�[��
class SceneGame:public Scene
{
public:
	SceneGame() {}
	~SceneGame() override{}

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
	Stage* stage = nullptr;

	std::unique_ptr<ICameraController> i_CameraController = nullptr;
	std::shared_ptr<Player> player;
	MiniMap* minimap = nullptr;
};
