#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "RenderContext.h"

// �X�v���C�g
class Sprite
{
public:
	Sprite();
	Sprite(const char* filename);

	// ���_�f�[�^
	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	struct MiniMap
	{
		int Flag;
		float radius;
		float paramatar;
		float dummy;
	};

	// �`����s
	void Render(const RenderContext& rc,
		float dx, float dy,					// ����ʒu
		float dz,							// ���s
		float dw, float dh,					// ���A����
		float sx, float sy,					// �摜�؂蔲���ʒu
		float sw, float sh,					// �摜�؂蔲���T�C�Y
		float angle,						// �p�x
		float r, float g, float b, float a	// �F
		, bool minimapFlg = false,
		float radius = 0.0f,//���a
		float parameter = 360.0f
	) const;

	// �`����s�i�e�N�X�`���؂蔲���w��Ȃ��j
	void Render(const RenderContext& rc,
		float dx, float dy,					// ����ʒu
		float dz,							// ���s
		float dw, float dh,					// ���A����
		float angle,						// �p�x
		float r, float g, float b, float a	// �F
	) const;

	//void DrawGui()const;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				minimap_constant_Buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;

	float textureWidth = 0;
	float textureHeight = 0;

	float radius = 200.0f;

	float parametar = 360.0f;
};
