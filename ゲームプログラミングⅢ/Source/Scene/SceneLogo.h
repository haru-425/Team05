#pragma once

#include"System/Sprite.h"
#include"Scene.h"
#include<thread>

//ローディングシーン
class SceneLogo : public Scene
{
public:
	SceneLogo(Scene* nextScene) :nextScene(nextScene) {}
	~SceneLogo() {}

	//初期化
	void Initialize() override;

	//終了化
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render()override;

	//GUI描画
	void DrawGUI()override;

private:
	//ローディングスレッド
	static void LoadingThread(SceneLogo* scene);

private:
	Sprite* sprite = nullptr;
	float angle = 0.0f;
	float positionX = 0;
	float positionY = 0;
	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;

	float timer;
	float transtimer;
	float nextSceneReadyTime = -1.0f; // 次のシーンの準備完了時刻
};