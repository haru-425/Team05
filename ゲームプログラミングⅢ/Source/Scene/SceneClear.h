#pragma once
#include"Scene.h"
#include"System/Sprite.h"
#include"Life.h"

class Game_Clear :public Scene
{
public:
	Game_Clear() {}
	~Game_Clear() {}

	//初期化
	void Initialize();

	//終了化
	void Finalize();

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render();

	//GUI描画
	void DrawGUI() {}
private:
	Sprite* GameOver;
	float GameCleartime = 0.0f; ///< デバッグ用タイマー 

	float timer = 0.0f; // タイマー
	float transTimer = 0.0f; // シーン遷移タイマー
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1秒後に遷移するシーン
};