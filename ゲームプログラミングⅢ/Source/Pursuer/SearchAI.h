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

	// DijkstraSearch(�_�C�N�X�g���T�[�`)
	bool DijkstraSearch(Stage* stage, bool heuristicFlg = false);
	// Dijkstra�Ŏg�p����R�X�g�v�Z�֐�
	Edge* searchMinCostEdge(std::vector<Edge*>& FNR, Stage* stage, bool heuristicFlg);
	// A*�Ŏg�p���錩�σR�X�g�v�Z�֐�
	float heuristicCulc(WayPoint* N1, WayPoint* N2);

	//�T�������G�b�W�����L�����郏�[�N
	std::vector<Edge*> searchEdge;

	//�������[�g�̃G�b�W�����L�����郏�[�N
	std::vector<int> findRoot;
	// �T�������f�[�^��\��
	//void SearchRender(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Stage06* stage);
	//void AddSerchArrow(Stage06* stage);
	void SearchClear(Stage* stage);
	//float AddAnswerArrow(Stage06* stage);
	//void GoldenPathSpawn(Stage06* stage, DirectX::XMFLOAT3 startPosition);

};

