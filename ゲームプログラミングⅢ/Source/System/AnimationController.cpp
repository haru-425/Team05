#include "AnimationController.h"

void AnimationController::UpdateAnimation(float dt)
{
    auto model = modelRef.lock();
    // モデルが見つからない場合は return 
    if (!model)
    {
        OutputDebugStringA("Not found target model.\n");
        return;
    }

    if (animationPlaying)
    {
        isEndAnimation = false;

        std::vector<Model::Node>& nodes = model->GetNodes();

        // 指定したアニメーションデータを取得
        const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
        const ModelResource::Animation& animation = animations.at(animationIndex);

        // 時間経過
        animationSecond += dt * animationSecondScale;

        // 再生時間が経過時間を超えたら
        float blendRate = 1.0f;

        if (animationSecond >= animation.secondsLength)
        {
            if (animationLoop)
            {
                animationSecond = 0;
                isEndAnimation = true;
            }
            else
            {
                animationPlaying = false;
                animationSecond = animation.secondsLength;
                isEndAnimation = true;
            }
        }

        if (animationSecond < animationBlendSecondLength)
        {
            blendRate = animationSecond / animationBlendSecondLength;
            blendRate *= blendRate;
        }

        // アニメーションデータからのキーフレーム
        const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
        int keyCount = static_cast<int>(keyframes.size());
        for (int keyIndex = 0; keyIndex < keyCount - 1; keyIndex++)
        {
            // 現在の時間がどのキーフレームの間にいるのか判定する
            const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
            const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
            if (animationSecond >= keyframe0.seconds && animationSecond < keyframe1.seconds)
            {
                // 再生時間とキーフレームの時間から補完率を算出
                float rate = (animationSecond - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

                // 全てのノードの姿勢を計算する
                int nodeCount = static_cast<int>(nodes.size());
                for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
                {
                    // キーフレームデータを取得
                    const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                    const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                    // ノード取得
                    Model::Node& node = nodes[nodeIndex];

                    using namespace DirectX;
                    if (blendRate < 1.0f)
                    {

                        // 前のキーフレームと次のキーフレームの姿勢を補完
                        XMVECTOR S0 = DirectX::XMLoadFloat3(&node.scale);
                        XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
                        XMVECTOR R0 = DirectX::XMLoadFloat4(&node.rotate);
                        XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
                        XMVECTOR T0 = DirectX::XMLoadFloat3(&node.translate);
                        XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

                        XMVECTOR S = DirectX::XMVectorLerp(S0, S1, blendRate);
                        XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, blendRate);
                        XMVECTOR T = DirectX::XMVectorLerp(T0, T1, blendRate);
                        // 計算結果をボーンに格納
                        XMStoreFloat3(&node.scale, S);
                        XMStoreFloat4(&node.rotate, R);
                        XMStoreFloat3(&node.translate, T);
                    }
                    else
                    {
                        // 前のキーフレームと次のキーフレームの姿勢を補完
                        XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
                        XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
                        XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
                        XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
                        XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
                        XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

                        XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
                        XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
                        XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);
                        // 計算結果をボーンに格納
                        XMStoreFloat3(&node.scale, S);
                        XMStoreFloat4(&node.rotate, R);
                        XMStoreFloat3(&node.translate, T);
                    }
                }
                break;
            }
        }
    }
}

void AnimationController::PlayAnimation(int index, bool loop)
{
    animationPlaying = true;
    animationLoop = loop;
    animationIndex = index;
    animationSecond = 0.0f;
}

void AnimationController::PlayAnimation(const char* name, bool loop)
{
    auto model = modelRef.lock();
    if (!model)
    {
        OutputDebugStringA("Not found target model.\n");
        return;
    }

    int index = 0;
    const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
    for (const ModelResource::Animation& animation : animations)
    {
        if (animation.name == name)
        {
            PlayAnimation(index, loop);
            return;
        }
        ++index;
    }
}