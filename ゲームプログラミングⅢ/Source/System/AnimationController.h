#pragma once
#include "System/Model.h"
#include <memory>

class AnimationController
{
public:
    AnimationController() = default;
    ~AnimationController() = default;

    // �A�j���[�V�����X�V����
    void UpdateAnimation(float dt);

    // �A�j���[�V�����Đ�
    void PlayAnimation(int index, bool loop);
    void PlayAnimation(const char* name, bool loop);

    // �Đ����郂�f�����o�C���h
    void SetModel(std::shared_ptr<Model> model) { modelRef = model; }

    // �A�j���[�V�������Đ����邩
    void SetAnimationPlaying(bool animationPlaying) { this->animationPlaying = animationPlaying; }

    void SetAnimationSecondScale(float animationSecondScale) { this->animationSecondScale = animationSecondScale; }

private:
    std::weak_ptr<Model> modelRef;

    int animationIndex = -1;
    float animationSecond = 0.0f;
    float animationSecondScale = 1.0f;
    bool animationLoop = false;
    bool animationPlaying = false;
    float animationBlendSecondLength = 0.2f;
};