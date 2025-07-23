#pragma once
#include"Scene.h"
#include"System/Sprite.h"
#include"Life.h"

class Game_Over :public Scene
{
public:
	Game_Over() {}
	Game_Over(int life):life_number(life){}
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

	int life_number;

	float timer = 0.0f; // タイマー
	float transTimer = 0.0f; // シーン遷移タイマー
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1秒後に遷移するシーン

	enum class SelectTrans {
		Title, // シーンクリア
		Game, // ゲームオーバー
		cnt,
	};

	SelectTrans selectTrans;
};