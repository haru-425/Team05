#pragma once

#include <DirectXMath.h>
#include "RenderState.h"

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;               // �f�o�C�X�R���e�L�X�g�B�`��R�}���h�̔��s�Ɏg�p�B
											         
	const RenderState* renderState = nullptr;         // ���݂̃����_�[�X�e�[�g�ւ̃|�C���^�i�u�����h�E���X�^���C�U�E�f�v�X�X�e�[�g�Ȃǁj
											         
	DirectX::XMFLOAT4X4 view;                         // �J�����̃r���[�s��i���[���h��ԁ��J������ԁj
	DirectX::XMFLOAT4X4 projection;                   // �J�����̎ˉe�s��i�J������ԁ��N���b�v��ԁj

	DirectX::XMFLOAT3 lightDirection = { 0, -1, 0 };  // ���s�����̕����x�N�g���i�ォ��Ƃ炷�j
	DirectX::XMFLOAT4 cameraPosition;                 // �J�����̃��[���h���W�i�s�N�Z���V�F�[�_���Ŏg�p�j

	// �V���h�E�}�b�v�֘A
	DirectX::XMFLOAT4X4 lightViewProjection;          // ���C�g���_����̃r���[�ˉe�s��i�V���h�E�}�b�s���O�p�j
	DirectX::XMFLOAT3 shadowColor;                    // �e�̐F�i��Z�I�Ɏg�p�j
	float shadowBias;                                 // �[�x�o�C�A�X�i�V���h�E�A�N�l��h�����߂̔������l�j

	// �t�H�O����ъ���
	DirectX::XMFLOAT4 ambientColor;                   // �A���r�G���g���C�g�i�����j�̐F
	DirectX::XMFLOAT4 fogColor;                       // �t�H�O�̐F
	DirectX::XMFLOAT4 fogRange;                       // �t�H�O�̊J�n�E�I�������ix: start, y: end, z/w: ���g�p�܂��̓J�X�^���p�r�j

	// �_�����ݒ�
	float lightPower;                                 // �S�̓I�Ȍ����̋��x�i�G�l���M�[�W���j

	struct PointLightConstants
	{
		DirectX::XMFLOAT4 position;                   // �_�����̈ʒu�iw�͖��g�p�j
		DirectX::XMFLOAT4 color;                      // ���̐F�i���x���܂߂Ē����\�j
		float range;                                  // �L�������i�����̏I�������j
		DirectX::XMFLOAT3 dummy = { 0,0,0 };          // �A���C�������g�����p�i16�o�C�g���E�j
	};

	PointLightConstants pointLights[47];               // �_�������ő�8�܂Œ�`�\

	// �������ݒ�
	struct LineLightConstants
	{
		DirectX::XMFLOAT4 start;                      // �������̎n�_
		DirectX::XMFLOAT4 end;                        // �������̏I�_
		DirectX::XMFLOAT4 color;                      // ���̐F
		float range;                                  // �L�������i������̋����Ō����j
		DirectX::XMFLOAT3 dummy = { 0,0,0 };          // �A���C�������g�����p
	};

	LineLightConstants lineLights[42];                 // ���������ő�8�܂Œ�`�\
};
