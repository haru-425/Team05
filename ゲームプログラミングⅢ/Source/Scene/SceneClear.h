#pragma once
#include"Scene.h"
#include"System/Sprite.h"
#include"Life.h"
#include "RankSystem/Ranksystem.h"
#include "System/AudioSource.h"
class Game_Clear :public Scene
{
public:
	Game_Clear() {}
	~Game_Clear() {}

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
	Sprite* s_rank;
	Sprite* s_result;
	Sprite* s_bg1;
	Sprite* s_bg2;
	float GameCleartime = 0.0f; ///< �f�o�b�O�p�^�C�}�[
	RankSystem::Rank result;

	float timer = 0.0f; // �^�C�}�[
	float transTimer = 0.0f; // �V�[���J�ڃ^�C�}�[
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1�b��ɑJ�ڂ���V�[��
	float angle;

	// �����֘A�ϐ�
	AudioSource* selectSE = nullptr;
};