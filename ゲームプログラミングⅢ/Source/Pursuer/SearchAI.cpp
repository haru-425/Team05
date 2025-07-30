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

				// �i�ݐ�̃m�[�h�܂ł̃R�X�g���v�Z
				float newCost = stage->wayPoint[nowEdge->destinationPoint]->costFromStart + nextEdge->defaultcost;

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

	//����������Ȃ����false;
	delete nowEdge;
	return false;
}


bool SearchAI::freeSearch(Stage* stage, bool heuristicFlg)
{
	tracking = false;
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

				// �i�ݐ�̃m�[�h�܂ł̃R�X�g���v�Z
				float newCost = stage->wayPoint[nowEdge->destinationPoint]->costFromStart + nextEdge->cost;

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

		// �R�X�g�v�Z
		// totalCost�ɐڑ����́u�X�^�[�g�ʒu����̃R�X�g�v�icostFromStart�j�{ �G�b�W���̂����R�X�g�icost�j������
		float totalCost = 0;
		if (tracking)
		{
			totalCost = stage->wayPoint[edge->originPoint]->costFromStart + edge->defaultcost;
		}
		else
		{
			totalCost = stage->wayPoint[edge->originPoint]->costFromStart + edge->cost;
		}

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
			// frontCost�Ɍ��σR�X�g�̉��Z
			// ���σR�X�g�̌v�Z�ɂ�heuristicCulc�֐����g�p���Ă��ǂ�
			frontCost += heuristicCulc(stage->wayPoint[edge->destinationPoint].get(), stage->wayPoint[stage->NearWayPointIndex(Goal::Instance().GetPosition())].get());
		}

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
	searchEdge.push_back(answer);//�T�[�`����Edge�̋L�^

	frontier.erase(frontier.begin() + answerNo);//�����̃G�b�W�̓_�C�N�X�g���̃T�[�`��₩��O��

	//����FNR�ɓo�^����̂͂��̖߂�l�ɓo�^����Ă���m�[�h�̃G�b�W�Q
	return answer;

}

float SearchAI::heuristicCulc(WayPoint* w1, WayPoint* w2)
{
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