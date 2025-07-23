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

	//������
	void Initialize();

	//�I����
	void Finalize();

	//�X�V����
	void Update(float elapsedTime);

	//�`�揈��
	void Render();

	//GUI�`��
	void DrawGUI() {}
private:
	Sprite* GameOver;
	Life* life[3];
	float GameOvertime = 0.0f; ///< �f�o�b�O�p�^�C�}�[

	int life_rest = 2;

	int life_number;

	float timer = 0.0f; // �^�C�}�[
	float transTimer = 0.0f; // �V�[���J�ڃ^�C�}�[
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1�b��ɑJ�ڂ���V�[��

	enum class SelectTrans {
		Title, // �V�[���N���A
		Game, // �Q�[���I�[�o�[
		cnt,
	};

	SelectTrans selectTrans;
};