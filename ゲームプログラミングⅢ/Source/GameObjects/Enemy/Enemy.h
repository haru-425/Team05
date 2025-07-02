#pragma once
#include "GameObject.h"
#include <memory>
#include "System/Model.h"
#include "Stage.h"

class Player;

class Enemy : public GameObject
{
public:
	Enemy() {}
	Enemy(std::shared_ptr<Player> playerRef, Stage* stage);
    ~Enemy();

    void Update(float dt) override ;

    void Render(const RenderContext& rc, ModelRenderer* renderer);

    void SetPlayer(std::shared_ptr<Player> player) { playerRef = player; }

	// 
	void Addroute(DirectX::XMFLOAT3 pos) { route.push_back(pos); }

	void Updatemovement(float elapsedTime);

	void DrawDebug() override;

	float GetPitch() const { return pitch; }
	float GetYaw() const { return yaw; }

private:
	enum class State
	{
		Wander,
		Idle,
		Attack,
		Roaming,
	};

private:
	Model* model = nullptr;
	std::weak_ptr<Player> playerRef;
	Stage* stage;

	State state = State::Roaming;
	std::vector<DirectX::XMFLOAT3> route;          // 移動ルート
	size_t currentTargetIndex = 0;        // 今向かっているポイントのインデックス
	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0, 0, 0 };
	float territoryRange = 10.0f;
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);

	float stateTimer = 0.0f;

	float searchRange = 5.0f;

	float pitch=0;
	float yaw=0;
};

