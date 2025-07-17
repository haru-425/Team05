#pragma once

#include <memory>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Model.h"
#include "Shader.h"

enum class ShaderId
{
	Basic,
	Lambert,
	Custom,

	EnumCount
};

class ModelRenderer
{
public:
	ModelRenderer(ID3D11Device* device);
	~ModelRenderer() {}

	// �`����s
	void Render(const RenderContext& rc, const DirectX::XMFLOAT4X4& worldTransform, const Model* model, ShaderId shaderId);

private:
	// �V�[���S�̂Ɋւ���萔�o�b�t�@
	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;     // �r���[�ˉe�s��i�J��������̎��_�j
		DirectX::XMFLOAT4		lightDirection;     // ���s�����̕����x�N�g��
		DirectX::XMFLOAT4       cameraPosition;     // �J�����̃��[���h���W
	};

	// �t�H�O��A���r�G���g���C�e�B���O�Ɋւ���萔
	struct fogConstants
	{
		DirectX::XMFLOAT4       ambientColor;       // �A���r�G���g���̐F
		DirectX::XMFLOAT4       fogColor;           // �t�H�O�̐F
		DirectX::XMFLOAT4       fogRange;           // �t�H�O�̊J�n�����E�I�������Ȃǁi�g�p���@�ɂ��j
	};

	// �_�����̏����i�[����\����
	struct PointLightConstants
	{
		DirectX::XMFLOAT4 position;                // �_�����̈ʒu
		DirectX::XMFLOAT4 color;                   // ���̐F
		float range;                               // �L�������i�����͈́j
		DirectX::XMFLOAT3 dummy;                   // �A���C�������g�����p�i16�o�C�g���E�ێ��j
	};

	struct TorusLightConstants
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 direction;
		DirectX::XMFLOAT4 color;
		float majorRadius;   // �h�[�i�c�̒��S�~�̔��a
		float minorRadius;   // �h�[�i�c�̑����i�f�ʉ~�̔��a�j
		float range;
		float dummy;
	};
	// �������iLine Light�j�̏����i�[����\����
	struct LineLightConstants
	{
		DirectX::XMFLOAT4 start;                   // �������̎n�_
		DirectX::XMFLOAT4 end;                     // �������̏I�_
		DirectX::XMFLOAT4 color;                   // ���̐F
		float range;                               // �L������
		DirectX::XMFLOAT3 dummy = { 0,0,0 };       // �A���C�������g�����p
	};

	// �S�̂̃��C�e�B���O�萔�i�_�����E�������Ȃǂ܂Ƃ߂ĊǗ��j
	struct LightConstants
	{
		PointLightConstants pointLights[256];        // �_�����̔z��i�ő�8�j
		TorusLightConstants torusLights[256];        // �~�����̔z��i�ő�8�j
		LineLightConstants lineLights[256];          // �������̔z��i�ő�8�j
		float power;                               // �Ɩ��S�̂̋��x�i�O���[�o���W���j
		DirectX::XMFLOAT3 dummy;                   // �A���C�������g�����p
	};

	// �V���h�E�}�b�s���O�p�̒萔
	struct ShadowConstants
	{
		DirectX::XMFLOAT4X4     lightViewProjection; // ���C�g���_����̃r���[�ˉe�s��
		DirectX::XMFLOAT3	    shadowColor;         // �e�̐F
		float				    shadowBias;          // �V���h�E�o�C�A�X�i�A�[�e�B�t�@�N�g����j
		DirectX::XMFLOAT4       edgeColor;
	};

	// �X�P���^���A�j���[�V�����p�̃{�[���ϊ��s��z��
	struct CbSkeleton
	{
		DirectX::XMFLOAT4X4		boneTransforms[256]; // �ő�256�̃{�[���s��
	};

	// �e��V�F�[�_�[�̃|�C���^�iEnumCount ���p�Ӂj
	std::unique_ptr<Shader>					shaders[static_cast<int>(ShaderId::EnumCount)];

	// �e�萔�o�b�t�@�iGPU�ɑ���f�[�^�j
	Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;     // �V�[�����i�r���[�E���������Ȃǁj
	Microsoft::WRL::ComPtr<ID3D11Buffer> skeletonConstantBuffer;  // �{�[���ϊ����
	Microsoft::WRL::ComPtr<ID3D11Buffer> fogConstantBuffer;       // �t�H�O�E�������
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowConstantBuffer;    // �V���h�E���
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;     // ���C�e�B���O���
};
