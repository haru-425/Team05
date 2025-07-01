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

    DestinationPointSet();
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



////////////////////////////////////////////////////////////////////////////////////////
//経路探索用

// ウェイポイントとエッジの設定
void Stage::DestinationPointSet()
{
    //WayPoint生成
    wayPoint[0] = std::make_shared<WayPoint>(0, DirectX::XMFLOAT3{ 0,0,0 });
    wayPoint[1] = std::make_shared<WayPoint>(1, DirectX::XMFLOAT3{ 15,0,9 });
    wayPoint[2] = std::make_shared<WayPoint>(2, DirectX::XMFLOAT3{ 10,0,2 });
    wayPoint[3] = std::make_shared<WayPoint>(3, DirectX::XMFLOAT3{ 4,0,12 });
    wayPoint[4] = std::make_shared<WayPoint>(4, DirectX::XMFLOAT3{ 22,0,22 });


    //
    wayPoint[0]->AddEdge(wayPoint[1].get());
    wayPoint[0]->AddEdge(wayPoint[3].get());
    wayPoint[1]->AddEdge(wayPoint[2].get());
    wayPoint[2]->AddEdge(wayPoint[4].get());
    wayPoint[3]->AddEdge(wayPoint[4].get());
}

// インデックス番号からウェイポイントの座標を取得
DirectX::XMFLOAT3 Stage::GetIndexWayPoint(int index)
{

    return wayPoint[index]->position;
}

// 座標から一番近いウェイポイントのインデックスを取得
int Stage::NearWayPointIndex(DirectX::XMFLOAT3 target)
{
    float minLength = FLT_MAX;
    float length = 0.0f;
    int index = -1;
    // VECTORに変換
    DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&target);

    for (int i = 0; i < MAX_WAY_POINT; ++i)
    {
        DirectX::XMVECTOR point = DirectX::XMLoadFloat3(&(wayPoint[i]->position));
        // 距離を求める
        DirectX::XMVECTOR vector = DirectX::XMVectorSubtract(targetPos, point);
        DirectX::XMVECTOR vectorLength = DirectX::XMVector3Length(vector);
        DirectX::XMStoreFloat(&length, vectorLength);

        // 求めた距離が保存しているものより小さければ
        if (minLength > length)
        {
            // 値を更新
            minLength = length;
            index = i;
        }
    }
    return index;
}