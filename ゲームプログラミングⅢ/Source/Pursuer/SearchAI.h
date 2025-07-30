#pragma once
#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <memory>
#include "Edge.h"
#include "WayPoint.h"
#include "Stage.h"
#include "ObjectBase.h"

#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

class SearchAI
{
public:
	SearchAI();
	~SearchAI();

	static SearchAI& Instance()
	{
		static SearchAI instance;
		return instance;
	}

	// DijkstraSearch(ダイクストラサーチ)
	bool trackingSearch(Stage* stage, bool heuristicFlg = false);
	bool freeSearch(Stage* stage, bool heuristicFlg = false);
	// Dijkstraで使用するコスト計算関数
	Edge* searchMinCostEdge(std::vector<Edge*>& FNR, Stage* stage, bool heuristicFlg);
	// A*で使用する見積コスト計算関数
	float heuristicCulc(WayPoint* N1, WayPoint* N2);

	//探索したエッジをを記憶するワーク
	std::vector<Edge*> searchEdge;

	//答えルートのエッジをを記憶するワーク
	std::vector<int> findRoot;
	void SearchClear(Stage* stage);

private:
	bool tracking = false;
};

