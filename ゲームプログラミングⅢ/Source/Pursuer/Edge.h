#pragma once
#include <DirectXMath.h>

class WayPoint;

class Edge
{
public:
    // �R���X�g���N�^
    Edge() : cost(0), originPoint(-1), destinationPoint(-1) {}
    // �f�X�g���N�^�i���ɏ����Ȃ��j
    ~Edge() {}

    int originPoint = -1;       // �Ӂi�G�b�W�j�̎n�_�̒��_�ԍ��i�����l�͖�����-1�j
    int destinationPoint = -1;  // �ӂ̏I�_�̒��_�ԍ��i�����l�͖�����-1�j
    float defaultcost = 0;
    float cost = 0;             // �ӂ̃R�X�g�i������d�݂Ȃǂ�\���j
};