#pragma once
#include <memory>
#include "Stage.h"
#include "ICameraController.h"
#include "FPCameraController.h"
#include "FreeCameraController.h"
#include "Scene.h"
#include <memory>
#include "Player/Player.h"
#include "Enemy/Enemy.h"

class SceneMattsu : public Scene
{
public:
	SceneMattsu() {}
	~SceneMattsu() override {}

	// ‰Šú‰»
	void Initialize()override;

	// I—¹‰»
	void Finalize()override;

	// XVˆ—
	void Update(float elapsedTime)override;

	// •`‰æˆ—
	void Render()override;

	// GUI•`‰æ
	void DrawGUI()override;

private:
	void Collision();

	void PlayerVsStage();

	void PlayerVsEnemy();

	void UpdateCamera(float dt);

private:
	Stage* stage = nullptr;

	std::unique_ptr<ICameraController> i_CameraController = nullptr;
	std::shared_ptr<Player> player;
	std::shared_ptr<Enemy> enemy;
};

