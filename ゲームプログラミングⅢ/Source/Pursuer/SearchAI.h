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
	bool DijkstraSearch(Stage* stage, bool heuristicFlg = false);
	// Dijkstraで使用するコスト計算関数
	Edge* searchMinCostEdge(std::vector<Edge*>& FNR, Stage* stage, bool heuristicFlg);
	// A*で使用する見積コスト計算関数
	float heuristicCulc(WayPoint* N1, WayPoint* N2);

	//探索したエッジをを記憶するワーク
	std::vector<Edge*> searchEdge;

	//答えルートのエッジをを記憶するワーク
	std::vector<int> findRoot;
	// 探索したデータを表示
	//void SearchRender(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Stage06* stage);
	//void AddSerchArrow(Stage06* stage);
	void SearchClear(Stage* stage);
	//float AddAnswerArrow(Stage06* stage);
	//void GoldenPathSpawn(Stage06* stage, DirectX::XMFLOAT3 startPosition);

};

