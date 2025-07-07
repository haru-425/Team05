#pragma once

#include "System/Sprite.h"
#include "Scene.h"

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
	Sprite* sprite = nullptr;
	float TitleTimer = 0.0f; // �^�C�g����ʂ̃^�C�}�[
	float TitleSignalTimer = 0.0f; // �^�C�g����ʂ̐M���^�C�}�[
	bool sceneTrans = false;
	Scene* nextScene = nullptr; ///< 1�b��ɑJ�ڂ���V�[��
};
