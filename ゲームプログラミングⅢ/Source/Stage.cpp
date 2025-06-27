#include"Stage.h"

//コンストラクタ
Stage::Stage()
{
    //ステージモデルを読み込み
#if 1
    model = new Model("Data/Model/Stage/stage.mdl");
#else
    model = new Model("Data/Model/Stage/ExampleStage.mdl");
#endif

    //scale = { 1.5f,1.5f,1.5f };
    //scale = { 2,2,2 };
    scale = { 1,1,1 };

    //スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.x);
    //回転行列
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    //位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&world, W);
}
Stage::~Stage()
{
    //ステージモデルを破棄
    delete model;
}
//更新処理
void Stage::Update(float elapsedTime)
{

}
//描画
void Stage::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    //レンダラにモデルを描画してもらう
    renderer->Render(rc, world, model, ShaderId::Lambert);
}