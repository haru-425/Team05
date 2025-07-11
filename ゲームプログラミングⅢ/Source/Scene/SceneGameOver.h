#pragma once
#include"Scene.h"
#include"System/Sprite.h"
#include"Life.h"

class Game_Over :public Scene
{
public:
	Game_Over() {}
	~Game_Over() {}

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
	Life* life[3];
	float GameOvertime = 0.0f; ///< デバッグ用タイマー

	int life_rest = 2;

	static int life_number;
};