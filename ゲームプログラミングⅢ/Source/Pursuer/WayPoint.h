#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "Edge.h"

class WayPoint {
public:

    // �m�[�h�̋�ԏ�̈ʒu�i3D���W�j
    DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    std::string name;
    std::vector<Edge*> edges;
    // �T���ς݂��ǂ����̃t���O�itrue�Ȃ�T���ς݁j
    bool searchFg = false;
    // �J�n�_���炱�̃m�[�h�܂ł̃R�X�g�i�����l�� -1.0f �Ŗ����B���Ӗ��j
    float costFromStart = -1.0f;

    int pointNo;
    // �R���X�g���N�^
    WayPoint() {}
    WayPoint(int no,DirectX::XMFLOAT3 position, const std::string& name = "") : pointNo(no),name(name), position(position) {}

    // �f�X�g���N�^�i�K�v�ɉ����ă��\�[�X����j
    ~WayPoint()
    {
        for (auto edge : edges)
        {
            delete edge;
        }
    }

    void AddEdge(WayPoint* destination, float custum_pluss_cost = 0) 
    {
        Edge* edge = new Edge();
        edge->originPoint = pointNo;
        edge->destinationPoint = destination->pointNo;
        DirectX::XMVECTOR cost = DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&destination->position), DirectX::XMLoadFloat3(&this->position)));
        edge->cost = DirectX::XMVectorGetX(cost);
        edge->cost += custum_pluss_cost;
        edges.push_back(edge);
    }

};
