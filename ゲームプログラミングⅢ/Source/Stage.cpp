#include"Stage.h"
#include <imgui.h>

//コンストラクタ
Stage::Stage()
{
	//ステージモデルを読み込み
#if 1
	model = new Model("Data/Model/Stage/map.mdl");
#else
	//model = new Model("Data/Model/Stage/ExampleStage.mdl");
#endif

	//scale = { 1.5f,1.5f,1.5f };
	//scale = { 2,2,2 };
	//scale = { 1,1,1 };

	//position = { -4,0,18 };
	scale = { 0.01f,0.01f,0.01f };
	angle.y = DirectX::XMConvertToRadians(180);

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

	textures = std::make_unique<LoadTextures>();
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
	textures->Set(rc);

	//レンダラにモデルを描画してもらう
	renderer->Render(rc, world, model, ShaderId::Custom);
	//renderer->Render(rc, world, model, ShaderId::Lambert);

	textures->Clear(rc);

}



////////////////////////////////////////////////////////////////////////////////////////
//経路探索用

// ウェイポイントとエッジの設定
void Stage::DestinationPointSet()
{
	//WayPoint生成
	wayPoint[0] = std::make_shared<WayPoint>(0, DirectX::XMFLOAT3{ 0,0,0 });
	wayPoint[1] = std::make_shared<WayPoint>(1, DirectX::XMFLOAT3{ 0,0,5 });
	wayPoint[2] = std::make_shared<WayPoint>(2, DirectX::XMFLOAT3{ 0,0,13 });
	wayPoint[3] = std::make_shared<WayPoint>(3, DirectX::XMFLOAT3{ 3.5,0,5 });
	wayPoint[4] = std::make_shared<WayPoint>(4, DirectX::XMFLOAT3{ 0,0,-4 });
	wayPoint[5] = std::make_shared<WayPoint>(5, DirectX::XMFLOAT3{ -8,0,5 });
	wayPoint[6] = std::make_shared<WayPoint>(6, DirectX::XMFLOAT3{ 8,0,13 });
	wayPoint[7] = std::make_shared<WayPoint>(7, DirectX::XMFLOAT3{ -8,0,13 });
	wayPoint[8] = std::make_shared<WayPoint>(8, DirectX::XMFLOAT3{ 5.5,0,5 });
	wayPoint[9] = std::make_shared<WayPoint>(9, DirectX::XMFLOAT3{ 3.5,0,0.5 });
	wayPoint[10] = std::make_shared<WayPoint>(10, DirectX::XMFLOAT3{ 4,0,-4 });
	wayPoint[11] = std::make_shared<WayPoint>(11, DirectX::XMFLOAT3{ -6,0,-4 });
	wayPoint[12] = std::make_shared<WayPoint>(12, DirectX::XMFLOAT3{ -8,0,2.5 });
	wayPoint[13] = std::make_shared<WayPoint>(13, DirectX::XMFLOAT3{ -13,0,5 });
	wayPoint[14] = std::make_shared<WayPoint>(14, DirectX::XMFLOAT3{ 8,0,22 });
	wayPoint[15] = std::make_shared<WayPoint>(15, DirectX::XMFLOAT3{ 14,0,13 });
	wayPoint[16] = std::make_shared<WayPoint>(16, DirectX::XMFLOAT3{ -8,0,22 });
	wayPoint[17] = std::make_shared<WayPoint>(17, DirectX::XMFLOAT3{ -13,0,13 });
	wayPoint[18] = std::make_shared<WayPoint>(18, DirectX::XMFLOAT3{ 7.5,0,5 });
	wayPoint[19] = std::make_shared<WayPoint>(19, DirectX::XMFLOAT3{ 5.5,0,0.5 });
	wayPoint[20] = std::make_shared<WayPoint>(20, DirectX::XMFLOAT3{ 6,0,-4 });
	wayPoint[21] = std::make_shared<WayPoint>(21, DirectX::XMFLOAT3{ -11,0,-4.5 });
	wayPoint[22] = std::make_shared<WayPoint>(22, DirectX::XMFLOAT3{ -13,0,2.5 });
	wayPoint[23] = std::make_shared<WayPoint>(23, DirectX::XMFLOAT3{ -21,0,5 });
	wayPoint[24] = std::make_shared<WayPoint>(24, DirectX::XMFLOAT3{ 3.5,0,22 });
	wayPoint[25] = std::make_shared<WayPoint>(25, DirectX::XMFLOAT3{ 21,0,13 });
	wayPoint[26] = std::make_shared<WayPoint>(26, DirectX::XMFLOAT3{ -2.5,0,22 });
	wayPoint[27] = std::make_shared<WayPoint>(27, DirectX::XMFLOAT3{ -17,0,13 });
	wayPoint[28] = std::make_shared<WayPoint>(28, DirectX::XMFLOAT3{ 9.5,0,5 });
	wayPoint[29] = std::make_shared<WayPoint>(29, DirectX::XMFLOAT3{ 7.5,0,0.5 });
	wayPoint[30] = std::make_shared<WayPoint>(30, DirectX::XMFLOAT3{ 8,0,-4 });
	wayPoint[31] = std::make_shared<WayPoint>(31, DirectX::XMFLOAT3{ -11,0,-12 });
	wayPoint[32] = std::make_shared<WayPoint>(32, DirectX::XMFLOAT3{ -16,0,-4 });
	wayPoint[33] = std::make_shared<WayPoint>(33, DirectX::XMFLOAT3{ -21,0,-4 });
	wayPoint[34] = std::make_shared<WayPoint>(34, DirectX::XMFLOAT3{ -29,0,5 });
	wayPoint[35] = std::make_shared<WayPoint>(35, DirectX::XMFLOAT3{ -21,0,13 });
	wayPoint[36] = std::make_shared<WayPoint>(36, DirectX::XMFLOAT3{ 1.5,0,22 });
	wayPoint[37] = std::make_shared<WayPoint>(37, DirectX::XMFLOAT3{ 29,0,13 });
	wayPoint[38] = std::make_shared<WayPoint>(38, DirectX::XMFLOAT3{ 21,0,5 });
	wayPoint[39] = std::make_shared<WayPoint>(39, DirectX::XMFLOAT3{ -0.5,0,22 });
	wayPoint[40] = std::make_shared<WayPoint>(40, DirectX::XMFLOAT3{ 15,0,5 });
	wayPoint[41] = std::make_shared<WayPoint>(41, DirectX::XMFLOAT3{ 7.5,0,0.5 });
	wayPoint[42] = std::make_shared<WayPoint>(42, DirectX::XMFLOAT3{ 11,0,-4.5 });
	wayPoint[43] = std::make_shared<WayPoint>(43, DirectX::XMFLOAT3{ -11,0,-13 });
	wayPoint[44] = std::make_shared<WayPoint>(44, DirectX::XMFLOAT3{ -21,0,-13 });
	wayPoint[45] = std::make_shared<WayPoint>(45, DirectX::XMFLOAT3{ -29,0,9 });
	wayPoint[46] = std::make_shared<WayPoint>(46, DirectX::XMFLOAT3{ -29,0,13 });
	wayPoint[47] = std::make_shared<WayPoint>(47, DirectX::XMFLOAT3{ 29,0,5 });
	wayPoint[48] = std::make_shared<WayPoint>(48, DirectX::XMFLOAT3{ 21,0,-4 });
	wayPoint[49] = std::make_shared<WayPoint>(49, DirectX::XMFLOAT3{ 16,0,-4 });
	wayPoint[50] = std::make_shared<WayPoint>(50, DirectX::XMFLOAT3{ 11,0,-12 });
	wayPoint[51] = std::make_shared<WayPoint>(51, DirectX::XMFLOAT3{ -6.5,0,-12 });
	wayPoint[52] = std::make_shared<WayPoint>(52, DirectX::XMFLOAT3{ -15,0,-13 });
	wayPoint[53] = std::make_shared<WayPoint>(53, DirectX::XMFLOAT3{ -21,0,-18 });
	wayPoint[54] = std::make_shared<WayPoint>(54, DirectX::XMFLOAT3{ -28,0,-13 });
	wayPoint[55] = std::make_shared<WayPoint>(55, DirectX::XMFLOAT3{ 21,0,-13 });
	wayPoint[56] = std::make_shared<WayPoint>(56, DirectX::XMFLOAT3{ 13,0,-13 });
	wayPoint[57] = std::make_shared<WayPoint>(57, DirectX::XMFLOAT3{ 11,0,-13 });
	wayPoint[58] = std::make_shared<WayPoint>(58, DirectX::XMFLOAT3{ 8.5,0,-12 });
	wayPoint[59] = std::make_shared<WayPoint>(59, DirectX::XMFLOAT3{ 8.5,0,-10 });
	wayPoint[60] = std::make_shared<WayPoint>(60, DirectX::XMFLOAT3{ 1,0,-12 });


	//
	wayPoint[0]->AddEdge(wayPoint[1].get());

	wayPoint[1]->AddEdge(wayPoint[2].get());
	wayPoint[1]->AddEdge(wayPoint[3].get());
	wayPoint[1]->AddEdge(wayPoint[4].get());
	wayPoint[1]->AddEdge(wayPoint[5].get());

	wayPoint[2]->AddEdge(wayPoint[6].get());
	wayPoint[2]->AddEdge(wayPoint[7].get());

	wayPoint[3]->AddEdge(wayPoint[8].get());
	wayPoint[3]->AddEdge(wayPoint[9].get());

	wayPoint[4]->AddEdge(wayPoint[10].get());
	wayPoint[4]->AddEdge(wayPoint[11].get());

	wayPoint[5]->AddEdge(wayPoint[12].get());
	wayPoint[5]->AddEdge(wayPoint[13].get());

	wayPoint[6]->AddEdge(wayPoint[14].get());
	wayPoint[6]->AddEdge(wayPoint[15].get());

	wayPoint[7]->AddEdge(wayPoint[16].get());
	wayPoint[7]->AddEdge(wayPoint[17].get());

	wayPoint[8]->AddEdge(wayPoint[18].get());
	wayPoint[8]->AddEdge(wayPoint[19].get());

	wayPoint[9]->AddEdge(wayPoint[19].get());

	wayPoint[10]->AddEdge(wayPoint[20].get());

	wayPoint[11]->AddEdge(wayPoint[21].get());

	wayPoint[12]->AddEdge(wayPoint[22].get());

	wayPoint[13]->AddEdge(wayPoint[22].get());
	wayPoint[13]->AddEdge(wayPoint[23].get());

	wayPoint[14]->AddEdge(wayPoint[24].get());

	wayPoint[15]->AddEdge(wayPoint[25].get());

	wayPoint[16]->AddEdge(wayPoint[26].get());

	wayPoint[17]->AddEdge(wayPoint[27].get());

	wayPoint[18]->AddEdge(wayPoint[28].get());
	wayPoint[18]->AddEdge(wayPoint[29].get());

	wayPoint[19]->AddEdge(wayPoint[29].get());

	wayPoint[20]->AddEdge(wayPoint[30].get());

	wayPoint[21]->AddEdge(wayPoint[31].get());
	wayPoint[21]->AddEdge(wayPoint[32].get());

	wayPoint[23]->AddEdge(wayPoint[33].get());
	wayPoint[23]->AddEdge(wayPoint[34].get());
	wayPoint[23]->AddEdge(wayPoint[35].get());

	wayPoint[24]->AddEdge(wayPoint[36].get());

	wayPoint[25]->AddEdge(wayPoint[37].get());
	wayPoint[25]->AddEdge(wayPoint[38].get());

	wayPoint[26]->AddEdge(wayPoint[39].get());

	wayPoint[27]->AddEdge(wayPoint[35].get());

	wayPoint[28]->AddEdge(wayPoint[40].get());
	wayPoint[28]->AddEdge(wayPoint[41].get());

	wayPoint[29]->AddEdge(wayPoint[41].get());

	wayPoint[30]->AddEdge(wayPoint[42].get());

	wayPoint[31]->AddEdge(wayPoint[43].get());
	wayPoint[31]->AddEdge(wayPoint[51].get());

	wayPoint[32]->AddEdge(wayPoint[33].get());

	wayPoint[33]->AddEdge(wayPoint[44].get());

	wayPoint[34]->AddEdge(wayPoint[45].get());

	wayPoint[35]->AddEdge(wayPoint[46].get());

	wayPoint[36]->AddEdge(wayPoint[39].get());

	wayPoint[37]->AddEdge(wayPoint[47].get());

	wayPoint[38]->AddEdge(wayPoint[47].get());
	wayPoint[38]->AddEdge(wayPoint[48].get());
	wayPoint[38]->AddEdge(wayPoint[40].get());

	wayPoint[42]->AddEdge(wayPoint[49].get());
	wayPoint[42]->AddEdge(wayPoint[50].get());

	wayPoint[43]->AddEdge(wayPoint[52].get());

	wayPoint[44]->AddEdge(wayPoint[53].get());
	wayPoint[44]->AddEdge(wayPoint[54].get());

	wayPoint[45]->AddEdge(wayPoint[46].get());

	wayPoint[48]->AddEdge(wayPoint[49].get());
	wayPoint[48]->AddEdge(wayPoint[55].get());
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
		if (minLength >= length)
		{
			// 値を更新
			minLength = length;
			index = i;
		}
	}
	return index;
}