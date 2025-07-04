#pragma once
#include "System/Model.h"
#include <memory>

class AnimationController
{
public:
    AnimationController() = default;
    ~AnimationController() = default;

    // アニメーション更新処理
    void UpdateAnimation(float dt);

    // アニメーション再生
    void PlayAnimation(int index, bool loop);
    void PlayAnimation(const char* name, bool loop);

    // 再生するモデルをバインド
    void SetModel(std::shared_ptr<Model> model) { modelRef = model; }

    // アニメーションを再生するか
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