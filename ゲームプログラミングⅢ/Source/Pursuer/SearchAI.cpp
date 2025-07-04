#include "SearchAI.h"

SearchAI::SearchAI()
{
	for (int i = 0; i < MAX_WAY_POINT; ++i)
	{
		findRoot.emplace_back(-1);
	}
}
SearchAI::~SearchAI()
{
	searchEdge.clear();
}

bool SearchAI::DijkstraSearch(Stage* stage, bool heuristicFlg)
{
	// 過去の探索データをクリア
	SearchClear(stage);

	//エッジ型データのフロンティアツリーを作成
	std::vector<Edge*> frontier;

	//スタートIDをoriginPoint ゴールIDをdistnationPointに持つダミーエッジを作成
	Edge* edge = debug_new Edge();

	//ダミーエッジは接続元と接続先は開始地点にしておく。
	edge->destinationPoint = stage->NearWayPointIndex(Start::Instance().GetPosition());
	edge->originPoint = edge->destinationPoint;
	// コスト0を指定(折り返しでスタート地点に戻らないようにするため)
	stage->wayPoint[edge->originPoint]->costFromStart = 0.0f;
	//前準備としてダミーエッジデータをセット
	Edge* nowEdge = edge;

	// DijkstraSseachアルゴリズム
	while (true)
	{
		//サーチしたEdgeの記録(実行結果の表示用）
		findRoot[nowEdge->destinationPoint] = nowEdge->originPoint;

		//デバッグ用
		int x = stage->NearWayPointIndex((Goal::Instance().GetPosition()));

		//もし、次のnowEdgeのdistnationNodeがゴールだったらtrueでreturnする。
		if (nowEdge->destinationPoint == stage->NearWayPointIndex((Goal::Instance().GetPosition())))
		{
			delete edge;
			return true;
		}

		//nowEdgeの先のノードを取得する。(今いるノード)
		WayPoint* wayPoint = stage->wayPoint[nowEdge->destinationPoint].get();

		//nowEdgeの先のノードに登録してある８本のエッジをサーチするループ
		for (auto edge : wayPoint->edges) {

			Edge* nextEdge = edge;

			// サーチするエッジが有効のとき
			if (nextEdge->destinationPoint >= 0 && nextEdge->destinationPoint < 400) {

				//進み先のノード
				WayPoint* nextPoint = stage->wayPoint[nextEdge->destinationPoint].get();

				//TODO 07_01
				// 進み先のノードまでのコストを計算
				float newCost = stage->wayPoint[nowEdge->destinationPoint]->costFromStart + nextEdge->cost;

				// TODO 07_02
				// 進み先のコストがまだ計算されていないか、新しいコストの方が低ければ
				// フロンティアツリーに登録
				// (計算されていないときコストには-1.0が入っている)
				if (nextPoint->costFromStart < 0 || nextPoint->costFromStart > newCost)
				{
					nextPoint->costFromStart = newCost;
					frontier.push_back(nextEdge);
				}

			}

		}

		nowEdge = searchMinCostEdge(frontier, stage, heuristicFlg);
		if (nowEdge->destinationPoint==0)
		{
			nowEdge = searchMinCostEdge(frontier, stage, heuristicFlg);
		}

		if (nowEdge == nullptr)
		{
			delete nowEdge;
			return false;
		}
	}

	//何も見つからなければfalse;
	delete nowEdge;
	return false;
}

Edge* SearchAI::searchMinCostEdge(std::vector<Edge*>& frontier, Stage* stage, bool heuristicFlg)
{
	Edge* answer = nullptr;//答えの入れ物
	int  answerNo = 0;
	float minCost = FLT_MAX;//最小のコストを保持するワーク

	for (int fnrNo = 0; fnrNo < frontier.size(); fnrNo++) {

		Edge* edge = frontier.at(fnrNo);

		// TODO 07_03
		// コスト計算
		// totalCostに接続元の「スタート位置からのコスト」（costFromStart）＋ エッジ自体が持つコスト（cost）を入れる
		float totalCost = 0;
		totalCost = stage->wayPoint[edge->originPoint]->costFromStart + edge->cost;

		// TODO 07_04
		// コスト取り出し
		//接続先の「スタート位置からのコスト」をfrontCostに取り出す(まだ登録されていないなら０となる)
		float frontCost = 0;
		if (stage->wayPoint[edge->destinationPoint]->costFromStart == 0)
		{
			frontCost = 0;
		}
		else
		{
			frontCost = stage->wayPoint[edge->destinationPoint]->costFromStart;
		}

		// TODO 07_05
		// コストの比較
		// frontCostが0(まだ登録されていない)か、frontCostより少ないコストルート(>=totalCost)が
		// 発見されたなら、接続先の「スタート位置からのコスト」（costFromStart）をtotalCostに書き換え。
		// frontCostもtotalCostに更新。
		if (frontCost == 0 || frontCost >= totalCost)
		{
			totalCost = stage->wayPoint[edge->destinationPoint]->costFromStart;
			frontCost = totalCost;
		}

		if (heuristicFlg)
		{
			// TODO 08_01
			// frontCostに見積コストの加算
			// 見積コストの計算にはheuristicCulc関数を使用しても良い
			frontCost += heuristicCulc(stage->wayPoint[edge->destinationPoint].get(), stage->wayPoint[stage->NearWayPointIndex(Goal::Instance().GetPosition())].get());
		}

		// TODO 07_06
		// エッジの記憶
		// minCostとfrontCostを比較し、frontCostが小さければminCostを更新。
		// 一番小さい接続先の「スタート位置からのコスト」を持つエッジを答え（answer）として記憶。
		// またanswerNoの更新も併せて行うこと
		if (minCost > frontCost)
		{
			minCost = frontCost;
			answer = edge;
			answerNo = fnrNo;
		}

	}

	if (answer == nullptr)
	{
		return 0;
	}
	searchEdge.push_back(answer);//サーチしたEdgeの記録(灰色ライン)

	frontier.erase(frontier.begin() + answerNo);//答えのエッジはダイクストラのサーチ候補から外す

	//次にFNRに登録するのはこの戻り値に登録されているノードのエッジ群
	return answer;

}

float SearchAI::heuristicCulc(WayPoint* w1, WayPoint* w2)
{
	// TODO 08_02
	// heuristicCulc関数を使用して見積コストの計算する場合は
	// 関数の内容を完成させること。
	// &w1->positionと&w2->positionの距離を計算
	// 計算した値を戻り値として返す
	DirectX::XMVECTOR start, end;
	start = DirectX::XMLoadFloat3(&w1->position);
	end = DirectX::XMLoadFloat3(&w2->position);

	DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(diff);

	return DirectX::XMVectorGetX(length);
}
//// 処理負荷が高すぎるので実装見送り
//void SearchAI::AddSerchArrow(Stage06* stage)
//{
//	for (const auto edge : searchEdge)
//	{
//		if (edge->destinationPoint < 0) continue;
//		// 探索したエッジを矢印で表示
//		Arrow06* arrow = new Arrow06();
//		arrow->SetPosition(stage->wayPoint[edge->originPoint]->position);
//		arrow->height = edge->cost;
//		arrow->color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
//		if (arrow->height > 2.0f)
//		{
//			arrow->height /= 5.0f;
//		}
//		arrow->radius = 0.05f;
//
//		// originPointからdestinationPointの角度を計算
//		DirectX::XMVECTOR origin = DirectX::XMLoadFloat3(&stage->wayPoint[edge->originPoint]->position);
//		DirectX::XMVECTOR destination = DirectX::XMLoadFloat3(&stage->wayPoint[edge->destinationPoint]->position);
//		DirectX::XMVECTOR vector = DirectX::XMVectorSubtract(destination, origin);
//		DirectX::XMFLOAT3 v, out;
//		DirectX::XMStoreFloat3(&v, vector);
//		float f = sqrtf(v.x * v.x + v.z * v.z);
//		out.x = 0.0f;
//		out.y = atan2f(v.z, -v.x);
//		out.z = atan2f(f, v.y);
//
//		arrow->SetAngle(out);
//		arrow->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
//		ArrowManager06::Instance().AddArrow(arrow);
//	}
//}
//float SearchAI::AddAnswerArrow(Stage06* stage)
//{
//	float totalCost = 0.0f;
//	// ゴール座標からインデックス番号を取得。ゴールから描画する
//	int startIndex = stage->NearWayPointIndex(Start06::Instance().GetPosition());
//	int startNo = stage->NearWayPointIndex(Goal06::Instance().GetPosition());
//	int endNo = -1;
//	while (endNo != startIndex) {
//		endNo = findRoot[startNo];
//		if (endNo == -1)break;
//
//		// arrowで描画する場合
//		// 探索したエッジを矢印で表示
//		Arrow06* arrow = new Arrow06();
//		arrow->SetPosition(stage->wayPoint[endNo]->position);
//
//		DirectX::XMVECTOR start, end;
//		start = DirectX::XMLoadFloat3(&stage->wayPoint[endNo]->position);
//		end = DirectX::XMLoadFloat3(&stage->wayPoint[startNo]->position);
//		DirectX::XMVECTOR vector = DirectX::XMVectorSubtract(end, start);
//		DirectX::XMVECTOR dist = DirectX::XMVector3Length(DirectX::XMVectorSubtract(end, start));
//		DirectX::XMFLOAT3 cost;
//		DirectX::XMStoreFloat3(&cost, dist);
//		arrow->height = cost.x;
//		arrow->radius = 0.05f;
//		arrow->color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
//		totalCost += cost.x;
//
//		// startからendの角度を計算
//		DirectX::XMFLOAT3 v, out;
//		DirectX::XMStoreFloat3(&v, vector);
//		float f = sqrtf(v.x * v.x + v.z * v.z);
//		out.x = 0.0f;
//		out.y = atan2f(v.z, -v.x);
//		out.z = atan2f(f, v.y);
//
//		arrow->SetAngle(out);
//		arrow->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
//		ArrowManager06::Instance().AddArrow(arrow);
//
//		startNo = endNo;
//	}
//	return totalCost;
//}
//void SearchAI::SearchRender(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Stage06* stage)
//{
//	LineRenderer* lineRenderer = Graphics::Instance().GetLineRenderer();
//
//	// サーチしたエッジを描画
//	for (const auto edge : searchEdge)
//	{
//		// 探索したエッジを描画
//		lineRenderer->AddVertex(DirectX::XMFLOAT3(stage->wayPoint[edge->originPoint]->position.x,
//			stage->wayPoint[edge->originPoint]->position.y + 2.0f,
//			stage->wayPoint[edge->originPoint]->position.z),
//			DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
//		lineRenderer->AddVertex(DirectX::XMFLOAT3(stage->wayPoint[edge->destinationPoint]->position.x,
//			stage->wayPoint[edge->destinationPoint]->position.y + 2.0f,
//			stage->wayPoint[edge->destinationPoint]->position.z),
//			DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
//
//	}
//
//	lineRenderer->Render(dc, view, projection);
//
//}
void SearchAI::SearchClear(Stage* stage)
{
	//ArrowManager::Instance().Clear();
	// 探索エッジをクリア
	searchEdge.clear();

	//サーチ済みのFGをリセット
	for (int i = 0; i < MAX_WAY_POINT; i++) {
		stage->wayPoint[i].get()->searchFg = false;
		stage->wayPoint[i].get()->costFromStart = -1.0f;
		findRoot[i] = -1;
	}
}
//
//void SearchAI::GoldenPathSpawn(Stage06* stage, DirectX::XMFLOAT3 startPosition)
//{
//	// 表示している敵を消す
//	EnemyManager06::Instance().Clear();
//
//	// TODO 09_01
//	// A*で探索した経路の周辺オブジェクトにプレイヤーから隠れるようにエネミーを1体配置させなさい。
//	// 全て自分で考えて実装しなさい。
//	// エネミーのスポーンコードは下記の通り。
//	// Enemy06* enemy = new Enemy06(出現させるウェイポイントを指定);
//	// EnemyManager06::Instance().AddEnemy(enemy);
//
//}