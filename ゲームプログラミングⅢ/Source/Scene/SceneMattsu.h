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

	// 初期化
	void Initialize()override;

	// 終了化
	void Finalize()override;

	// 更新処理
	void Update(float elapsedTime)override;

	// 描画処理
	void Render()override;

	// GUI描画
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

