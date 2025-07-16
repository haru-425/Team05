#include "ObjectBase.h"

void ObjectBase::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列を作成
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// 位置行列を作成
	DirectX::XMMATRIX t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// 3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX w = s * r * t;
	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, w);
}

//インスタンス取得
Start& Start::Instance() {
	static Start instance;
	return instance;
}


// コンストラクタ
Start::Start() {
	position = DirectX::XMFLOAT3(0.5f, 0.0f, 0.5f);
}

// デストラクタ
Start::~Start()
{
}

// プレイヤー更新処理
void Start::Update(float elapsedTime)
{
	// オブジェクト行列を更新
	UpdateTransform();

}

// 描画
void Start::Render(RenderContext rc, ShapeRenderer* renderer)
{
	// 描画
	//PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	/*DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);*/

	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
	//primitiveRenderer->DrawSphere(transform, radius, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
}

//インスタンス取得
Goal& Goal::Instance() {
	static Goal instance;
	return instance;
}


// コンストラクタ
Goal::Goal()
{
	position = DirectX::XMFLOAT3(19.5f, 0.0f, 19.5f);
}

// デストラクタ
Goal::~Goal()
{
}

// 描画
void Goal::Render(RenderContext rc, ShapeRenderer* renderer)
{
	// 描画
	//PrimitiveRenderer* primitiveRenderer = Graphics::Instance().GetPrimitiveRenderer();
	/*DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);*/

	renderer->RenderCylinder(rc, position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
	//primitiveRenderer->DrawSphere(transform, radius, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
}
