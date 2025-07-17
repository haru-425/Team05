#pragma once

#include "Scene.h"

//�V�[���}�l�[�W���[
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}

public:
    //�B��̃C���X�^���X�擾
    static SceneManager& instance()
    {
        static SceneManager instance;
        return instance;
    }

    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render();

    //GUI�`��
    void DrawGUI();

    //�V�[���N���A
    void Clear();

    //�V�[���؂�ւ�
    void ChangeScene(Scene* scene);

    void SetIsExit(bool isExit) { this->isExit = isExit; }
    bool GetIsExit() const { return isExit; }

private:
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;

    bool isExit = false;
};