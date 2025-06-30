#pragma once
#include "GameObject.h"
#include <memory>
#include "System/Model.h"

class Player;

class Enemy : public GameObject
{
public:
    Enemy();
    ~Enemy();

    void Update(float dt) override ;

    void Render(const RenderContext& rc, ModelRenderer* renderer);

    void SetPlayer(std::shared_ptr<Player> player) { playerRef = player; }

	// 
	void Addroute(DirectX::XMFLOAT3 pos) { route.push_back(pos); }

	void Updatemovement(float elapsedTime);
private:
	enum class State
	{
		Wander,
		Idle,
		Attack,
	};

private:
	Model* model = nullptr;
	std::weak_ptr<Player> playerRef;

	State state = State::Wander;
	std::vector<DirectX::XMFLOAT3> route;          // �ړ����[�g
	size_t currentTargetIndex = 0;        // ���������Ă���|�C���g�̃C���f�b�N�X
	DirectX::XMFLOAT3 targetPosition = { 0, 0, 0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0, 0, 0 };
	float territoryRange = 10.0f;
	float moveSpeed = 2.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);

	float stateTimer = 0.0f;

	float searchRange = 5.0f;
};

