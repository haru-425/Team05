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
	// �ߋ��̒T���f�[�^���N���A
	SearchClear(stage);

	//�G�b�W�^�f�[�^�̃t�����e�B�A�c���[���쐬
	std::vector<Edge*> frontier;

	//�X�^�[�gID��originPoint �S�[��ID��distnationPoint�Ɏ��_�~�[�G�b�W���쐬
	Edge* edge = debug_new Edge();

	//�_�~�[�G�b�W�͐ڑ����Ɛڑ���͊J�n�n�_�ɂ��Ă����B
	edge->destinationPoint = stage->NearWayPointIndex(Start::Instance().GetPosition());
	edge->originPoint = edge->destinationPoint;
	// �R�X�g0���w��(�܂�Ԃ��ŃX�^�[�g�n�_�ɖ߂�Ȃ��悤�ɂ��邽��)
	stage->wayPoint[edge->originPoint]->costFromStart = 0.0f;
	//�O�����Ƃ��ă_�~�[�G�b�W�f�[�^���Z�b�g
	Edge* nowEdge = edge;

	// DijkstraSseach�A���S���Y��
	while (true)
	{
		//�T�[�`����Edge�̋L�^(���s���ʂ̕\���p�j
		findRoot[nowEdge->destinationPoint] = nowEdge->originPoint;

		//�f�o�b�O�p
		int x = stage->NearWayPointIndex((Goal::Instance().GetPosition()));

		//�����A����nowEdge��distnationNode���S�[����������true��return����B
		if (nowEdge->destinationPoint == stage->NearWayPointIndex((Goal::Instance().GetPosition())))
		{
			delete edge;
			return true;
		}

		//nowEdge�̐�̃m�[�h���擾����B(������m�[�h)
		WayPoint* wayPoint = stage->wayPoint[nowEdge->destinationPoint].get();

		//nowEdge�̐�̃m�[�h�ɓo�^���Ă���W�{�̃G�b�W���T�[�`���郋�[�v
		for (auto edge : wayPoint->edges) {

			Edge* nextEdge = edge;

			// �T�[�`����G�b�W���L���̂Ƃ�
			if (nextEdge->destinationPoint >= 0 && nextEdge->destinationPoint < 400) {

				//�i�ݐ�̃m�[�h
				WayPoint* nextPoint = stage->wayPoint[nextEdge->destinationPoint].get();

				//TODO 07_01
				// �i�ݐ�̃m�[�h�܂ł̃R�X�g���v�Z
				float newCost = stage->wayPoint[nowEdge->destinationPoint]->costFromStart + nextEdge->cost;

				// TODO 07_02
				// �i�ݐ�̃R�X�g���܂��v�Z����Ă��Ȃ����A�V�����R�X�g�̕����Ⴏ���
				// �t�����e�B�A�c���[�ɓo�^
				// (�v�Z����Ă��Ȃ��Ƃ��R�X�g�ɂ�-1.0�������Ă���)
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

	//����������Ȃ����false;
	delete nowEdge;
	return false;
}

Edge* SearchAI::searchMinCostEdge(std::vector<Edge*>& frontier, Stage* stage, bool heuristicFlg)
{
	Edge* answer = nullptr;//�����̓��ꕨ
	int  answerNo = 0;
	float minCost = FLT_MAX;//�ŏ��̃R�X�g��ێ����郏�[�N

	for (int fnrNo = 0; fnrNo < frontier.size(); fnrNo++) {

		Edge* edge = frontier.at(fnrNo);

		// TODO 07_03
		// �R�X�g�v�Z
		// totalCost�ɐڑ����́u�X�^�[�g�ʒu����̃R�X�g�v�icostFromStart�j�{ �G�b�W���̂����R�X�g�icost�j������
		float totalCost = 0;
		totalCost = stage->wayPoint[edge->originPoint]->costFromStart + edge->cost;

		// TODO 07_04
		// �R�X�g���o��
		//�ڑ���́u�X�^�[�g�ʒu����̃R�X�g�v��frontCost�Ɏ��o��(�܂��o�^����Ă��Ȃ��Ȃ�O�ƂȂ�)
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
		// �R�X�g�̔�r
		// frontCost��0(�܂��o�^����Ă��Ȃ�)���AfrontCost��菭�Ȃ��R�X�g���[�g(>=totalCost)��
		// �������ꂽ�Ȃ�A�ڑ���́u�X�^�[�g�ʒu����̃R�X�g�v�icostFromStart�j��totalCost�ɏ��������B
		// frontCost��totalCost�ɍX�V�B
		if (frontCost == 0 || frontCost >= totalCost)
		{
			totalCost = stage->wayPoint[edge->destinationPoint]->costFromStart;
			frontCost = totalCost;
		}

		if (heuristicFlg)
		{
			// TODO 08_01
			// frontCost�Ɍ��σR�X�g�̉��Z
			// ���σR�X�g�̌v�Z�ɂ�heuristicCulc�֐����g�p���Ă��ǂ�
			frontCost += heuristicCulc(stage->wayPoint[edge->destinationPoint].get(), stage->wayPoint[stage->NearWayPointIndex(Goal::Instance().GetPosition())].get());
		}

		// TODO 07_06
		// �G�b�W�̋L��
		// minCost��frontCost���r���AfrontCost�����������minCost���X�V�B
		// ��ԏ������ڑ���́u�X�^�[�g�ʒu����̃R�X�g�v�����G�b�W�𓚂��ianswer�j�Ƃ��ċL���B
		// �܂�answerNo�̍X�V�������čs������
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
	searchEdge.push_back(answer);//�T�[�`����Edge�̋L�^(�D�F���C��)

	frontier.erase(frontier.begin() + answerNo);//�����̃G�b�W�̓_�C�N�X�g���̃T�[�`��₩��O��

	//����FNR�ɓo�^����̂͂��̖߂�l�ɓo�^����Ă���m�[�h�̃G�b�W�Q
	return answer;

}

float SearchAI::heuristicCulc(WayPoint* w1, WayPoint* w2)
{
	// TODO 08_02
	// heuristicCulc�֐����g�p���Č��σR�X�g�̌v�Z����ꍇ��
	// �֐��̓��e�����������邱�ƁB
	// &w1->position��&w2->position�̋������v�Z
	// �v�Z�����l��߂�l�Ƃ��ĕԂ�
	DirectX::XMVECTOR start, end;
	start = DirectX::XMLoadFloat3(&w1->position);
	end = DirectX::XMLoadFloat3(&w2->position);

	DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(diff);

	return DirectX::XMVectorGetX(length);
}
//// �������ׂ���������̂Ŏ���������
//void SearchAI::AddSerchArrow(Stage06* stage)
//{
//	for (const auto edge : searchEdge)
//	{
//		if (edge->destinationPoint < 0) continue;
//		// �T�������G�b�W����ŕ\��
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
//		// originPoint����destinationPoint�̊p�x���v�Z
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
//	// �S�[�����W����C���f�b�N�X�ԍ����擾�B�S�[������`�悷��
//	int startIndex = stage->NearWayPointIndex(Start06::Instance().GetPosition());
//	int startNo = stage->NearWayPointIndex(Goal06::Instance().GetPosition());
//	int endNo = -1;
//	while (endNo != startIndex) {
//		endNo = findRoot[startNo];
//		if (endNo == -1)break;
//
//		// arrow�ŕ`�悷��ꍇ
//		// �T�������G�b�W����ŕ\��
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
//		// start����end�̊p�x���v�Z
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
//	// �T�[�`�����G�b�W��`��
//	for (const auto edge : searchEdge)
//	{
//		// �T�������G�b�W��`��
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
	// �T���G�b�W���N���A
	searchEdge.clear();

	//�T�[�`�ς݂�FG�����Z�b�g
	for (int i = 0; i < MAX_WAY_POINT; i++) {
		stage->wayPoint[i].get()->searchFg = false;
		stage->wayPoint[i].get()->costFromStart = -1.0f;
		findRoot[i] = -1;
	}
}
//
//void SearchAI::GoldenPathSpawn(Stage06* stage, DirectX::XMFLOAT3 startPosition)
//{
//	// �\�����Ă���G������
//	EnemyManager06::Instance().Clear();
//
//	// TODO 09_01
//	// A*�ŒT�������o�H�̎��ӃI�u�W�F�N�g�Ƀv���C���[����B���悤�ɃG�l�~�[��1�̔z�u�����Ȃ����B
//	// �S�Ď����ōl���Ď������Ȃ����B
//	// �G�l�~�[�̃X�|�[���R�[�h�͉��L�̒ʂ�B
//	// Enemy06* enemy = new Enemy06(�o��������E�F�C�|�C���g���w��);
//	// EnemyManager06::Instance().AddEnemy(enemy);
//
//}