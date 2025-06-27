#pragma once

#include"System/Sprite.h"
#include"Scene.h"
#include<thread>

//���[�f�B���O�V�[��
class SceneLoading : public Scene
{
public:
    SceneLoading(Scene* nextScene):nextScene(nextScene){}
    ~SceneLoading(){}

    //������
    void Initialize() override;

    //�I����
    void Finalize()override;

    //�X�V����
    void Update(float elapsedTime) override;

    //�`�揈��
    void Render()override;

    //GUI�`��
    void DrawGUI()override;

private:
    //���[�f�B���O�X���b�h
    static void LoadingThread(SceneLoading* scene);

private:
    Sprite* sprite = nullptr;
    float angle = 0.0f;
    float positionX = 0;
    float positionY = 0;
    Scene* nextScene = nullptr;
    std::thread* thread = nullptr;
};