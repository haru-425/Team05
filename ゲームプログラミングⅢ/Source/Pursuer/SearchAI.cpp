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

bool SearchAI::trackingSearch(Stage* stage, bool heuristicFlg)
{
	tracking = true;
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

				// 進み先のノードまでのコストを計算
				float newCost = stage->wayPoint[nowEdge->destinationPoint]->costFromStart + nextEdge->defaultcost;

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
		if (nowEdge == nullptr)
		{
			delete nowEdge;
			return false;
		}
		if (nowEdge->destinationPoint == 0)
		{
			nowEdge = searchMinCostEdge(frontier, stage, heuristicFlg);
		}
	}

	//何も見つからなければfalse;
	delete nowEdge;
	return false;
}


bool SearchAI::freeSearch(Stage* stage, bool heuristicFlg)
{
	tracking = false;
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

				// 進み先のノードまでのコストを計算
				float newCost = stage->wayPoint[nowEdge->destinationPoint]->costFromStart + nextEdge->cost;

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
		if (nowEdge == nullptr)
		{
			delete nowEdge;
			return false;
		}
		if (nowEdge->destinationPoint == 0)
		{
			nowEdge = searchMinCostEdge(frontier, stage, heuristicFlg);
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

		// コスト計算
		// totalCostに接続元の「スタート位置からのコスト」（costFromStart）＋ エッジ自体が持つコスト（cost）を入れる
		float totalCost = 0;
		if (tracking)
		{
			totalCost = stage->wayPoint[edge->originPoint]->costFromStart + edge->defaultcost;
		}
		else
		{
			totalCost = stage->wayPoint[edge->originPoint]->costFromStart + edge->cost;
		}

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
			// frontCostに見積コストの加算
			// 見積コストの計算にはheuristicCulc関数を使用しても良い
			frontCost += heuristicCulc(stage->wayPoint[edge->destinationPoint].get(), stage->wayPoint[stage->NearWayPointIndex(Goal::Instance().GetPosition())].get());
		}

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
	searchEdge.push_back(answer);//サーチしたEdgeの記録

	frontier.erase(frontier.begin() + answerNo);//答えのエッジはダイクストラのサーチ候補から外す

	//次にFNRに登録するのはこの戻り値に登録されているノードのエッジ群
	return answer;

}

float SearchAI::heuristicCulc(WayPoint* w1, WayPoint* w2)
{
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