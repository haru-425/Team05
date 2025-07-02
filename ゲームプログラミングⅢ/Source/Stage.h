#pragma once

#include"System/ModelRenderer.h"
#include "Pursuer/WayPoint.h"
#include "Pursuer/Edge.h"
#include "Pursuer/Object.h"
#define MAX_WAY_POINT 5

//�X�e�[�W
class Stage
{
public:
    Stage();
    ~Stage();

    //�X�e�[�W
    void Update(float elapsedTime);

    //�`�揈��
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    //�o�H�T���p
    // �E�F�C�|�C���g�̃C���f�b�N�X����|�W�V�������擾
    DirectX::XMFLOAT3 GetIndexWayPoint(int index);
    // �w����W�����ԋ߂��E�F�C�|�C���g�̃C���f�b�N�X���擾
    int NearWayPointIndex(DirectX::XMFLOAT3 target);
    // �ڑ���|�C���g��ݒ�
    void DestinationPointSet();

    std::shared_ptr<WayPoint> wayPoint[MAX_WAY_POINT];
    std::vector<int> path;

    DirectX::XMFLOAT4X4 GetWorld() const { return world; }
    Model* GetModel() { return model; }

private:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 world = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };

    Model* model = nullptr;
};
