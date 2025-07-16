#pragma once
#include <DirectXMath.h>
#include "System/ModelRenderer.h"
#include "System/ShapeRenderer.h"

class ObjectBase
{
public:
	ObjectBase() {}
	virtual ~ObjectBase() {}

	// 行列更新処理
	void UpdateTransform();

	// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	// 位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	// 回転取得
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }

	// 回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	// スケール取得
	const DirectX::XMFLOAT3& GetScale()const { return scale; }

	// スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// 半径取得
	float GetRadius() const { return radius; }

	// 高さ取得
	float GetHeight()const { return height; };

protected:
	DirectX::XMFLOAT3	position = { 0,0,0 };
	DirectX::XMFLOAT3	angle = { 0,0,0, };
	DirectX::XMFLOAT3	scale = { 1,1,1 };
	DirectX::XMFLOAT4X4	transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	float	radius = 0.3f;
	float	height = 2.0f;

private:
};

// Start
class Start :public ObjectBase
{
public:
	// コンストラクタ
	Start();

	// デストラクタ
	~Start() override;

	static Start& Instance();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render(RenderContext rc, ShapeRenderer* renderer);

private:
	float				moveSpeed = 5.0f;
};

// ゴール
class Goal :public ObjectBase
{
public:
	// コンストラクタ
	Goal();

	// デストラクタ
	~Goal() override;

	static Goal& Instance();

	// 描画処理
	void Render(RenderContext rc, ShapeRenderer* renderer);
};

